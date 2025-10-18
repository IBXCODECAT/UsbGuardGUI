#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCoreApplication>
#include <QObject>
#include <QDebug>
#include <QQmlContext>

#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusMetaType>

#include "UsbGuardDevicesmodel.h"
#include "UsbGuardManager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // 1. Instantiate the Model
    UsbGuardDevicesModel* devicesModel = new UsbGuardDevicesModel(&app);

    // 2. Instantiate the Manager
    UsbGuardManager* manager = new UsbGuardManager(devicesModel, &app);

    // 3. Set up the QML Engine
    QQmlApplicationEngine engine;

    // 4. Expose the Model to QML using a Context Property
    // This is the line that caused the error (line 39 in your project):
    engine.rootContext()->setContextProperty("usbDevicesModel", devicesModel);

    // 5. Load the QML source
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("UsbGuardGUI", "Main");

    return app.exec();
}
