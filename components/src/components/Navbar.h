#pragma once

#include <QObject>
#include "services/network/MainNetwork.h"

class Backend;

class NavBar : public QObject {
    Q_OBJECT
public:
    explicit NavBar(Backend* b, QObject *parent = nullptr);

    Q_INVOKABLE bool checkInternet();
private:
    Backend* backend;
};