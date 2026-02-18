#ifndef OPENINGANIMATION_H
#define OPENINGANIMATION_H

#include <QObject>
#include "services/database/SqlDatabase.h"

class Backend;

class OpeningAnimation : public QObject {
    Q_OBJECT
public:
    explicit OpeningAnimation(Backend* b, QObject *parent = nullptr);
    
    Q_INVOKABLE bool checkUsernameAvailability();
    Q_INVOKABLE void insertUsername(const QString &username, const QString &storageLink);
    Q_INVOKABLE QString getUsername();
private:
    Backend* backend;
};

#endif //OPENINGANIMATION_H