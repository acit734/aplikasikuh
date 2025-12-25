#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#ifdef Q_OS_WIN
#include <windows.h>
#include <cstdio>
#include <io.h>
#include <fcntl.h>
#endif

#include "components/src/Backend.h"

int main(int argc, char *argv[])
{
    #ifdef Q_OS_WIN
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    // (opsional) redirect stdin: freopen("CONIN$", "r", stdin);
    #endif
    QGuiApplication app(argc, argv);
    Backend backend;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("Backend", &backend);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    
    QObject::connect(
        &app,
        &QCoreApplication::aboutToQuit,
        &backend,
        [&backend]() { backend.closeDb(); }
    );

    engine.loadFromModule("Proyek_1_moduled", "Main");

    return app.exec();
}
