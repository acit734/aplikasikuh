#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include "services/database/SqlDatabase.h"
#include "services/network/MainNetwork.h"
#include "services/network/GithubConnection.h"
#include "components/OpeningAnimation.h"
#include "components/Navbar.h"

class Backend : public QObject {
    Q_OBJECT
    Q_PROPERTY(QObject* openingAnimation READ openingAnimation CONSTANT)
    Q_PROPERTY(QObject* navBar READ navBar CONSTANT)
public: 
    explicit Backend(QObject *parent = nullptr);
    Q_INVOKABLE void debug(const QString text);

    void closeDb();
    
    SqlDatabase& getDb();
    MainNetwork& getMainNetwork();
    GithubConnection& getGithubConnection();

    QObject* openingAnimation() const;
    QObject* navBar() const;
private:
    SqlDatabase* m_db;
    MainNetwork* m_mainNetwork;
    GithubConnection* m_githubConnection;
    OpeningAnimation* m_openingAnimation;
    NavBar* m_navBar;
};

#endif // BACKEND_H