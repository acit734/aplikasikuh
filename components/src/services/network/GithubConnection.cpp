#include "GithubConnection.h"

#include <format>
#include <QDebug>
#include <QObject>
#include <QTimer>
#include <QFile>
#include <QIODevice>
#include <QCoreApplication>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "Backend.h"
#include "services/database/SqlCondition.h"
#include "services/database/SqlConditionBuilder.h"

GithubConnection::GithubConnection(Backend* b, QObject *parent) : QObject(parent), backend(b) {
    m_status = "Not Connected";
    m_manager = new QNetworkAccessManager(this);
    m_timer.setInterval(1000);
    m_timer.start();

    connect(&m_timer, &QTimer::timeout, this, GithubConnection::checkUser);
}

void GithubConnection::debug(const QVariant &obj) {
    qDebug() << "[GithubConnection]:" << obj;
    return;
}

void GithubConnection::checkUser() {
    if (!backend->getMainNetwork().online()) {
        m_timer.start();
        return;
    }
    QVariantList users = backend->getDb().selectWhere("users", expr("loggedIn", "=", 1));
    if (users.isEmpty()) {
        m_timer.start();
        return;
    }

    m_timer.stop();
    m_status = "Syncing";
    emit statusChanged();

    m_key = users[0].toMap()["storageLink"].toString();
    findLink();
}

void GithubConnection::findLink() {
    QNetworkRequest req(QUrl("https://api.github.com/user/repos?visibility=private&permission=push"));
    req.setRawHeader("Authorization", "Bearer " + m_key.toUtf8());
    req.setRawHeader("User-Agent", "ChromeTest");
    QNetworkReply *reply = m_manager->get(req);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        int code = reply->attribute(
            QNetworkRequest::HttpStatusCodeAttribute
        ).toInt();

        if (code == 200) {
            QByteArray raw = reply->readAll();
            QJsonDocument body = QJsonDocument::fromJson(raw);
            
            if (body.isArray()) {
                QString link = body.array()[0].toObject()["contents_url"].toString().replace("{+path}", "");
                m_storageLink = link;
                sync();
            }
        } else {
            debug(QString::number(code));
        }

        reply->deleteLater();
    });
}

bool GithubConnection::sync() {
    debug(m_storageLink);

    QFile databaseFile(QCoreApplication::applicationDirPath() + "/data/app.db");
    databaseFile.open(QIODevice::ReadOnly);

    QByteArray databaseData = databaseFile.readAll();
    QByteArray databaseBase64 = databaseData.toBase64();

    QNetworkRequest req(QUrl(m_storageLink.toUtf8() + "app.db"));
    req.setRawHeader("Authorization", "Bearer " + m_key.toUtf8());
    req.setRawHeader("Accept", "application/vnd.github+json");
    req.setRawHeader("User-Agent", "ChromeTest");
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject body;
    body["message"] = "database sync";
    body["content"] = QString::fromLatin1(databaseBase64);

    auto checkSha = [this, body, req]() mutable {
        QNetworkRequest shaReq(QUrl(m_storageLink.toUtf8() + "app.db"));
        shaReq.setRawHeader("Authorization", "Bearer " + m_key.toUtf8());
        shaReq.setRawHeader("User-Agent", "ChromeTest");
        QNetworkReply *shaReply = m_manager->get(shaReq);

        connect(shaReply, &QNetworkReply::finished, this, [this, shaReply, body, req]() mutable {
            int code = shaReply->attribute(
                QNetworkRequest::HttpStatusCodeAttribute
            ).toInt();

            if (code == 200) {
                QByteArray raw = shaReply->readAll();
                QJsonDocument doc = QJsonDocument::fromJson(raw);
                QString theSha = doc.object()["sha"].toString();
                body["sha"] = theSha;
                debug(theSha);
                debug(body["message"].toString());
                debug(body["content"].toString());
                debug(body["sha"].toString());

                QByteArray payload = QJsonDocument(body).toJson();
                QNetworkReply *putReply = m_manager->put(req, payload);

                connect(putReply, &QNetworkReply::finished, this, [this, putReply]() mutable {
                    debug(QString("PUT reply: %1").arg(putReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()));
                    putReply->deleteLater();
                });
            }
            shaReply->deleteLater();
        });
    };

    checkSha();
    return true;
}

void GithubConnection::onStatusChanged() {
    backend->navBar();
}