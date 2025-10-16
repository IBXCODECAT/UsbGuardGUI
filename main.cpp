#include <QGuiApplication> // Changed from QGuiApplication to QApplication
#include <QQmlApplicationEngine>
#include <QCoreApplication>
#include <QObject>

#include "tablemodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<TableModel>("TableModel", 0, 1, "TableModel");

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("UsbGuardGUI", "Main");

    return app.exec();
}
