#include "NavBar.h"

#include <QDebug>
#include "Backend.h"

NavBar::NavBar(Backend* b, QObject* parent) : QObject(parent), backend(b) {

}

QString NavBar::githubConnectionStatus() const {
    return m_githubConnectionStatus;
}

void NavBar::setGithubConnectionStatus(const QString &status) {
    if (m_githubConnectionStatus == status)
        return;
    m_githubConnectionStatus = status;
    emit githubConnectionStatusChanged();
}

bool NavBar::checkInternet() {
    return backend->getMainNetwork().online();
}
