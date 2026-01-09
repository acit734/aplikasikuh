#include "Backend.h"
#include "components/OpeningAnimation.h"
#include "services/database/SqlDatabase.h"
#include <QObject>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>

Backend::Backend(QObject *parent) : QObject(parent) {
    m_openingAnimation = new OpeningAnimation(this);
    m_db = new SqlDatabase(this);

    QString basePath = QCoreApplication::applicationDirPath();
    QString dataPath = basePath + "/data";

    if (!QDir().exists(dataPath)) QDir().mkpath(dataPath);

    m_db->open(dataPath + "/app.db");
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

QObject* Backend::openingAnimation() const {
    return m_openingAnimation;
}