#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include "services/database/SqlDatabase.h"
#include "components/OpeningAnimation.h"

class Backend : public QObject {
    Q_OBJECT
public: 
    explicit Backend(QObject *parent = nullptr);
    SqlDatabase& getDb();
    void closeDb();

    Q_INVOKABLE void debug(const QString text);
    Q_INVOKABLE OpeningAnimation* openingAnimation;
private:
    SqlDatabase* db;
};

#endif // BACKEND_H