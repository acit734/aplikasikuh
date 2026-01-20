#include "NavBar.h"

#include <QDebug>
#include "Backend.h"

NavBar::NavBar(Backend* b, QObject* parent) : QObject(parent), backend(b) {

}

bool NavBar::checkInternet() {
    return backend->getMainNetwork().online();
}
