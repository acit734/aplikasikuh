#include "Backend.h"
#include "components/OpeningAnimation.h"
#include "services/database/SqlDatabase.h"
#include <QObject>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>

Backend::Backend(QObject *parent) : QObject(parent) {
    openingAnimation = new OpeningAnimation(this);
    db = new SqlDatabase(this);

    QString basePath = QCoreApplication::applicationDirPath();
    QString dataPath = basePath + "/data";

    if (!QDir().exists(dataPath)) QDir().mkpath(dataPath);

    db->open(dataPath + "/app.db");
}

void Backend::debug(const QString text) {
    qDebug() << "[backend]: " << text;
}

SqlDatabase& Backend::getDb() {
    return *db;
}

void Backend::closeDb() {
    db->close();
    return;
}