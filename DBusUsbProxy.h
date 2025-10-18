#ifndef DBUSUSBPROXY_H
#define DBUSUSBPROXY_H

#include <QObject>
#include <QtDBus/QDBusConnection> // Added for QDBusConnection type
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusMetaType>
#include <QtDBus/QDBusReply>
#include <QtDBus/QDBusPendingCallWatcher> // Forward declaration is enough, but adding is clean.

#include <memory>

#include "DataStructures.h"

using InterfacePtr = std::unique_ptr<QDBusInterface>;

class DBusUsbProxy : public QObject
{
    Q_OBJECT

public:
    explicit DBusUsbProxy(QObject *parent = nullptr);

    // Public accessors to verify connections
    inline bool IsConnected() const { return m_sysBus.isConnected(); }
    inline bool IsInterfaceValid() const { return m_usbInterface != nullptr && m_usbInterface->isValid(); }

    // Public method to start the asynchronous D-Bus call
    void listDevices();

signals:
    // Signals to notify the GUI application
    void deviceListReady(const QList<UsbDeviceData>& devices);
    void errorOccurred(const QString& message);
    void deviceListChanged();

private slots:
    // Slot to handle the asynchronous reply from D-Bus
    void handleListDevicesReply(QDBusPendingCallWatcher *const watcher);

    // Slot to handle the DevicePolicyChanged D-Bus signal (u, u, u, s, u, a{ss})
    void handleRemotePolicyChanged(
        uint id,
        uint target_old,
        uint target_new,
        const QString& device_rule,
        uint rule_id,
        const QMap<QString, QString> &attributes);


private:
    QDBusConnection m_sysBus;
    InterfacePtr m_usbInterface = nullptr;

    // Helper function to encapsulate the complex rule parsing logic
    QList<UsbDeviceData> parseRuleStrings(const QList<QPair<uint, QString>>& rawData);
};

#endif // DBUSUSBPROXY_H
