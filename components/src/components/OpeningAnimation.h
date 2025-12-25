#ifndef OPENINGANIMATION_H
#define OPENINGANIMATION_H

#include <QObject>
#include "services/database/SqlDatabase.h"

class Backend;

class OpeningAnimation : public QObject {
    Q_OBJECT
    public:
    explicit OpeningAnimation(Backend* b, QObject *parent = nullptr);
    
    bool checkUsernameAvailability();
private:
    Backend* backend;
};

#endif //OPENINGANIMATION_H