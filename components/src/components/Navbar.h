#pragma once

#include <QObject>
#include "services/network/MainNetwork.h"

class Backend;

class NavBar : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString githubConnectionStatus READ githubConnectionStatus NOTIFY githubConnectionStatusChanged)
public:
    explicit NavBar(Backend* b, QObject *parent = nullptr);

    QString githubConnectionStatus() const;

    Q_INVOKABLE bool checkInternet();
    void setGithubConnectionStatus(const QString &status);
signals:
    void githubConnectionStatusChanged();
private:
    Backend* backend;

    QString m_githubConnectionStatus;
};