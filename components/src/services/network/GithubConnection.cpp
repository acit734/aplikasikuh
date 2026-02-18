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
                debug(QString("Server Content: %1").arg(serverContent.length()));
                debug(QString("Local Content: %1").arg(body["content"].toString().length()));
                mergeDb(QByteArray::fromBase64(serverContent.toUtf8()), QByteArray::fromBase64(body["content"].toString().toUtf8()));
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

void GithubConnection::mergeDb(const QByteArray &serverBinary, const QByteArray &localBinary) {
    QString tempFolder = QCoreApplication::applicationDirPath() + "/temp";
    if (!QDir().exists(tempFolder)) QDir().mkpath(tempFolder);

    // QFile serverFile(tempFolder + "/server.db");
    // if (!serverFile.open(QIODevice::WriteOnly)) {
    //     debug("Failed to make server.db");
    // }
    // serverFile.write(serverBinary);
    // serverFile.close();

    // QFile localFile(tempFolder + "/local.db");
    // if (!localFile.open(QIODevice::WriteOnly)) {
    //     debug("Failed to make local.db");
    // }
    // localFile.write(localBinary);
    // localFile.close();

    QSqlDatabase serverDb = QSqlDatabase::addDatabase("QSQLITE", "server_db");
    serverDb.setDatabaseName(tempFolder + "/server.db");
    serverDb.open();

    QSqlDatabase localDb = QSqlDatabase::addDatabase("QSQLITE", "local_db");
    localDb.setDatabaseName(tempFolder + "/local.db");
    localDb.open();

    QSqlDatabase mergedDb = QSqlDatabase::addDatabase("QSQLITE", "merged_db");
    mergedDb.setDatabaseName(tempFolder + "/merged.Db");
    mergedDb.open();

    QSqlQuery serverQuery(serverDb);
    QSqlQuery localQuery(localDb);
    QSqlQuery mergedQuery(mergedDb);

    serverQuery.exec("SELECT * FROM sqlite_master");
    localQuery.exec("SELECT * FROM sqlite_master");

    QVariantList serverTables;
    QVariantList localTables;

    while (serverQuery.next()) {
        QVariantMap row;
        QSqlRecord record = serverQuery.record();

        for (int i = 0; i < record.count(); i++)
            row.insert(record.fieldName(i), serverQuery.value(i));
        
        serverTables.append(row);
    }

    while (localQuery.next()) {
        QVariantMap row;
        QSqlRecord record = localQuery.record();

        for (int i = 0; i < record.count(); i++)
            row.insert(record.fieldName(i), localQuery.value(i));
        
        localTables.append(row);
    }

    debug(localTables);

    serverDb.close();
    serverDb = QSqlDatabase();
    QSqlDatabase::removeDatabase("server_db");

    localDb.close();
    localDb = QSqlDatabase();
    QSqlDatabase::removeDatabase("local_db");

    mergedDb.close();
    mergedDb = QSqlDatabase();
    QSqlDatabase::removeDatabase("merged_db");
}