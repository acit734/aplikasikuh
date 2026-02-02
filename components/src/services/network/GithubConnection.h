#pragma once

#include <QObject>
#include <QTimer>
#include <QtNetwork/QNetworkAccessManager>

class Backend;

class GithubConnection : public QObject {
    Q_OBJECT
public:
    explicit GithubConnection(Backend* b, QObject *parent = nullptr);
    void debug(const QVariant &obj);

    void checkUser();
    void findLink();
    bool sync();
signals:
    void statusChanged();
private slots:
    void onStatusChanged();
private:
    Backend *backend;

    QNetworkAccessManager *m_manager;
    QString m_linkToFile;
    QTimer m_timer;
    QString m_status;
    QString m_key;
    QString m_storageLink;
};