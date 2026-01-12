#include <QDebug>
#include "OpeningAnimation.h"
#include "services/database/SqlDatabase.h"
#include "Backend.h"
#include "services/database/SqlConditionBuilder.h"
#include <QJsonArray>
#include <QJsonDocument>

OpeningAnimation::OpeningAnimation(Backend* b, QObject *parent) : QObject(parent), backend(b) {

}

bool OpeningAnimation::checkUsernameAvailability() {
    SqlDatabase &db = backend->getDb();

    if (!db.isOpen()) {
        qWarning() << "[OpeningAnimation] Database is not open";
        return false;
    }

    QVariantList users = db.selectWhere("users", expr("loggedIn", "=", 1));
    if (users.isEmpty()) {
        qDebug() << "[OpeningAnimation] Table is empty";
        return false;
    }
    else return true;
}

void OpeningAnimation::insertUsername(const QString &username, const QString &storageLink) {
    const QString table = "users";
    QVariantMap data;

    data["username"] = username;
    data["storageLink"] = storageLink;
    data["loggedIn"] = true;

    SqlDatabase &db = backend->getDb();
    db.insert(table, data, "OpeningAnimation");

    return;
}

QString OpeningAnimation::getUsername() {
    QVariantList users = backend->getDb().selectWhere("users", expr("loggedIn", "=", 1));
    QVariant user = users[0];
    
    return user.toMap()["username"].toString();
}