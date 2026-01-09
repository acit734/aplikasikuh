#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include "services/database/SqlDatabase.h"
#include "components/OpeningAnimation.h"

class Backend : public QObject {
    Q_OBJECT
    Q_PROPERTY(QObject* openingAnimation READ openingAnimation CONSTANT)
public: 
    explicit Backend(QObject *parent = nullptr);
    SqlDatabase& getDb();
    void closeDb();

    Q_INVOKABLE void debug(const QString text);
    QObject* openingAnimation() const;
private:
    OpeningAnimation* m_openingAnimation;
    SqlDatabase* m_db;
};

#endif // BACKEND_H