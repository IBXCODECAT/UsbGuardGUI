#include "UsbGuardManager.h"
#include <QDebug>

UsbGuardManager::UsbGuardManager(UsbGuardDevicesModel* model, QObject *parent)
    : QObject(parent), m_model(model)
{
    // 1. Instantiate the D-Bus Proxy
    // The proxy connects to the D-Bus in its constructor.
    m_proxy = new DBusUsbProxy(this);

    // 2. Wire the Signals and Slots

    // A. When the proxy has the full, parsed list, give it to the model.
    connect(m_proxy, &DBusUsbProxy::deviceListReady,
            this, &UsbGuardManager::handleDeviceListReady);

    // B. When the D-Bus signals a change, re-request the full list to update.
    connect(m_proxy, &DBusUsbProxy::deviceListChanged,
            this, &UsbGuardManager::handleDeviceListChanged);

    // C. Handle errors (optional, but good practice).
    connect(m_proxy, &DBusUsbProxy::errorOccurred,
            this, &UsbGuardManager::handleProxyError);

    // 3. Initial Data Load
    refreshDeviceList();
}

void UsbGuardManager::refreshDeviceList()
{
    qDebug() << "Manager requesting device list from D-Bus proxy...";
    m_proxy->listDevices();
}

void UsbGuardManager::handleDeviceListReady(const QList<UsbDeviceData>& devices)
{
    qDebug() << "Manager received new device list. Passing to model...";
    m_model->loadDeviceData(devices);
}

void UsbGuardManager::handleDeviceListChanged()
{
    qDebug() << "D-Bus DeviceChanged signal received. Triggering full list refresh...";
    // The easiest way to handle a remote change is to re-request the full list.
    // A more complex solution would be to parse the D-Bus signal directly
    // and update the model's internal data without a full reset.
    refreshDeviceList();
}

void UsbGuardManager::handleProxyError(const QString& message)
{
    qCritical() << "USBGuard Proxy Error:" << message;
    // You could also emit a signal here to notify the main GUI window.
}
