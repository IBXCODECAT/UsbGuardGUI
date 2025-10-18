#ifndef DBUSUSBPROXY_H
#define DBUSUSBPROXY_H

#include <QObject>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusMetaType>
#include <QtDBus/QDBusReply>

#include <memory>

#include "DataStructures.h"

class DBusUsbProxy : public QObject
{
    Q_OBJECT

public:
    explicit DBusUsbProxy(QObject *parent = nullptr);

    // Public methods to verify connections and interface setup
    inline bool IsConnected() { return m_sysBus.isConnected(); }
    inline bool IsInterfaceValid() { return m_usbInterface != nullptr && m_usbInterface->isValid(); }

    // Public method to start the asynchronous D-Bus call
    void listDevices();

signals:
    // Signals to notify the GUI application
    void deviceListReady(const QList<UsbDeviceData>& devices);
    void errorOccurred(const QString& message);

    // Example signal for device events (D-Bus signals)
    void deviceListChanged();



private slots:
    // Slot to handle the asynchronous reply from D-Bus
    void handleListDevicesReply(QDBusPendingCallWatcher *watcher);

    // Feature not yet implemented by USBGuard
    // void handleRemoteDeviceChanged(
    //     uint id,
    //     uint event,
    //     const QString& target,
    //     const QString& device_rule,
    //     const QVariantMap &attributes // <-- a{ss}
    // );

    // SIGNATURE: u, u, u, s, u, a{ss}
    void handleRemotePolicyChanged(
        uint id,
        uint target_old,
        uint target_new,
        const QString& device_rule,
        uint rule_id,
        const QMap<QString, QString> &attributes); // <-- CHANGED TO QMap<QString, QString>


private:
    QDBusConnection m_sysBus;
    std::unique_ptr<QDBusInterface> m_usbInterface = nullptr;

    // Helper function to encapsulate the complex rule parsing logic
    QList<UsbDeviceData> parseRuleStrings(const QList<QPair<uint, QString>>& rawData);
};

#endif // DBUSUSBPROXY_H
