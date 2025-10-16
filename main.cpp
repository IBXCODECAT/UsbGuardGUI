#include <QGuiApplication> // Changed from QGuiApplication to QApplication
#include <QQmlApplicationEngine>
#include <QCoreApplication>
#include <QObject>

#include "UsbGuardDevicesmodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Register the UsbGuardDeviceModel
    qmlRegisterType<UsbGuardDevicesModel>("UsbGuardDevicesModel", 1, 0, "UsbGuardDevicesModel");

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
