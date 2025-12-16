#include "Backend.h"
#include <QObject>
#include <QDebug>

Backend::Backend(QObject *parent) : QObject(parent) {}

void Backend::debug(const QString text) {
    qDebug() << "[backend]: " << text;
}