#ifndef USBGUARDMANAGER_H
#define USBGUARDMANAGER_H

#include <QObject>
#include "DBusUsbProxy.h"

#include "UsbGuardDevicesmodel.h"

class UsbGuardManager : public QObject
{
    Q_OBJECT
public:
    explicit UsbGuardManager(UsbGuardDevicesModel* model, QObject *parent = nullptr);

    // Public method for the GUI to request a refresh
    void refreshDeviceList();

private slots:
    // Slot to receive the parsed device list from the proxy
    void handleDeviceListReady(const QList<UsbDeviceData>& devices);

    // Slot to handle a notification that the list has changed on the D-Bus
    void handleDeviceListChanged();

    // Slot to handle errors from the proxy
    void handleProxyError(const QString& message);

private:
    UsbGuardDevicesModel* m_model;
    DBusUsbProxy* m_proxy;
};

#endif // USBGUARDMANAGER_H
