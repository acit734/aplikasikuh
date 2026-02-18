#include "GithubConnection.h"

#include <format>
#include <QDebug>
#include <QObject>
#include <QTimer>
#include <QFile>
#include <QDir>
#include <QIODevice>
#include <QCoreApplication>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "Backend.h"
#include "services/database/SqlCondition.h"
#include "services/database/SqlConditionBuilder.h"

GithubConnection::GithubConnection(Backend* b, QObject *parent) : QObject(parent), backend(b) {
    m_status = "Not Connected";
    backend->getNavBar().setGithubConnectionStatus(m_status);
    m_manager = new QNetworkAccessManager(this);
    m_timer.setInterval(1000);
    m_timer.setSingleShot(false);
    m_timer.start();

    connect(&m_timer, &QTimer::timeout, this, &GithubConnection::checkUser);
}

void GithubConnection::debug(const QVariant &obj, bool showType) {
    if (obj.canConvert<QString>() && !showType) 
        qDebug() << "[GithubConnection]:" << obj.toString().toUtf8();
    else if ((obj.canConvert<QVariantList>() || obj.canConvert<QVariantMap>()) && !showType) {
        QJsonValue value = QJsonValue::fromVariant(obj);
        
        QJsonDocument doc;
        if (value.isObject()) doc = QJsonDocument(value.toObject());
        if (value.isArray()) doc = QJsonDocument(value.toArray());

        qDebug() << "[GithubConnection]:" << doc.toJson(QJsonDocument::Compact);
    } else {
        qDebug() << "[GithubConnection]:" << obj;
    }
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
    backend->getNavBar().setGithubConnectionStatus(m_status);

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
    body["content"] = QString::fromUtf8(databaseBase64);

    auto asyncWork = [this, body, req]() mutable {
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

                QString serverContent = doc.object()["content"].toString().remove("\n");
            }
            QByteArray payload = QJsonDocument(body).toJson();
            QNetworkReply *putReply = m_manager->put(req, payload);

            connect(putReply, &QNetworkReply::finished, this, [this, putReply]() mutable {
                int code = putReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

                    if (code == 200) m_status = "Synced";
                else m_status = "Failed to Sync";
                backend->getNavBar().setGithubConnectionStatus(m_status);
                putReply->deleteLater();
            });
            shaReply->deleteLater();
        });
    };

    asyncWork();
    return true;
}