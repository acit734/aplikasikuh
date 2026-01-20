#include "Backend.h"
#include "components/OpeningAnimation.h"
#include "components/Navbar.h"
#include "services/database/SqlDatabase.h"
#include "services/network/MainNetwork.h"
#include <QObject>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>

Backend::Backend(QObject *parent) : QObject(parent) {
    m_openingAnimation = new OpeningAnimation(this);
    m_navBar = new NavBar(this);

    m_db = new SqlDatabase(this);
    m_mainNetwork = new MainNetwork(this);

    QString basePath = QCoreApplication::applicationDirPath();
    QString dataPath = basePath + "/data";

    if (!QDir().exists(dataPath)) QDir().mkpath(dataPath);

    m_db->open(dataPath + "/app.db");

    m_mainNetwork->start();
}

void Backend::debug(const QString text) {
    qDebug() << "[Backend]:" << text;
}

SqlDatabase& Backend::getDb() {
    return *m_db;
}

void Backend::closeDb() {
    m_db->close();
    return;
}

MainNetwork& Backend::getMainNetwork() {
    return *m_mainNetwork;
}

QObject* Backend::openingAnimation() const {
    return m_openingAnimation;
}

QObject* Backend::navBar() const {
    return m_navBar;
}