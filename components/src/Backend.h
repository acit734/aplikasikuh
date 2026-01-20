#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include "services/database/SqlDatabase.h"
#include "services/network/MainNetwork.h"
#include "components/OpeningAnimation.h"
#include "components/Navbar.h"

class Backend : public QObject {
    Q_OBJECT
    Q_PROPERTY(QObject* openingAnimation READ openingAnimation CONSTANT)
    Q_PROPERTY(QObject* navBar READ navBar CONSTANT)
public: 
    explicit Backend(QObject *parent = nullptr);
    SqlDatabase& getDb();
    void closeDb();

    MainNetwork& getMainNetwork();

    Q_INVOKABLE void debug(const QString text);
    QObject* openingAnimation() const;
    QObject* navBar() const;
private:
    SqlDatabase* m_db;
    MainNetwork* m_mainNetwork;
    OpeningAnimation* m_openingAnimation;
    NavBar* m_navBar;
};

#endif // BACKEND_H