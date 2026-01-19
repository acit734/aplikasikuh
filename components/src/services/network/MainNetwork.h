#pragma once

#include <QObject>
#include <QTimer>
#include <QtNetwork/QNetworkReply>

class MainNetwork : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool online READ online NOTIFY onlineChanged)
public:
    explicit MainNetwork(QObject *parent = nullptr);

    void start();
    void stop();
    bool online() const;
signals:
    void onlineChanged();
private:
    void checkInternet();
    void onReply(QNetworkReply *reply);
    QTimer m_timer;
    QNetworkAccessManager m_manager;
    bool m_checking;
    bool m_online;
};