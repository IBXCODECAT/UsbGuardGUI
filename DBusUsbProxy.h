#ifndef DBUSUSBPROXY_H
#define DBUSUSBPROXY_H

#include <QObject>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusMetaType>
#include <QtDBus/QDBusReply>

struct UsbDeviceData
{
    uint id = 0;
    QString status; // e.g ALLOW, BLOCK, REJECT
    QString name;   // User-friendly name if availible
    QString vidPid; // Vendor ID and Product ID string (e.g., "1234:5678")
    QString serial; // Device Serial Number
};

Q_DECLARE_METATYPE(UsbDeviceData)

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

    // Slot to handle remote D-Bus signals (e.g., when a device is plugged/unplugged)
    // NOTE: The signature must match the remote D-Bus signal exactly.
    void handleRemoteDeviceChanged(uint id, const QString& rule);

private:
    QDBusConnection m_sysBus;
    std::unique_ptr<QDBusInterface> m_usbInterface = nullptr;

    // Helper function to encapsulate the complex rule parsing logic
    QList<UsbDeviceData> parseRuleStrings(const QList<QPair<uint, QString>>& rawData);
};

#endif // DBUSUSBPROXY_H
