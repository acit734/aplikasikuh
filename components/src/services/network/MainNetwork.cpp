#include "MainNetwork.h"
#include <QDebug>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

MainNetwork::MainNetwork(QObject *parent) : QObject(parent), m_online(false), m_checking(false) {
    m_timer.setInterval(1000);
    m_timer.setSingleShot(false);
    
    connect(&m_timer, &QTimer::timeout, this, MainNetwork::checkInternet);
    connect(&m_manager, &QNetworkAccessManager::finished, this, MainNetwork::onReply);
}

void MainNetwork::start() {
    checkInternet();
    m_timer.start();
}

void MainNetwork::stop() {
    m_timer.stop();
}

bool MainNetwork::online() const {
    return m_online;
}

void MainNetwork::checkInternet() {
    if (m_checking) return;
    m_checking = true;
    QNetworkRequest req(QUrl("https://www.google.com/generate_204"));
    req.setTransferTimeout(3000);
    m_manager.get(req);
}

void MainNetwork::onReply(QNetworkReply *reply) {
    bool status = (reply->error() == QNetworkReply::NoError);

    if (m_online != status) {
        m_online = status;
        emit onlineChanged();
    }

    m_checking = false;
    reply->deleteLater();
}

