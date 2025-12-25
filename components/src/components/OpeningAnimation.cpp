#include <QDebug>
#include "OpeningAnimation.h"
#include "services/database/SqlDatabase.h"
#include "Backend.h"

OpeningAnimation::OpeningAnimation(Backend* b, QObject *parent) : QObject(parent), backend(b) {

}

bool OpeningAnimation::checkUsernameAvailability() {
    SqlDatabase &db = backend->getDb();

    if (!db.isOpen()) {
        qWarning() << "[OpeningAnimation] Database is not open";
        return false;
    }

    QVariantList users = db.selectAll("users");
    if (users.isEmpty()) return false;
    else return true;
}