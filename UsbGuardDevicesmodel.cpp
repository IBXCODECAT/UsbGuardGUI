#include "UsbGuardDevicesmodel.h"

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusReply>

#include <QtDBus/QDBusMetaType>

UsbGuardDevicesModel::UsbGuardDevicesModel(QObject *parent) : TableModelBase(parent)
{
    initializeHeaders();
    connectToDBus();
}

void UsbGuardDevicesModel::initializeHeaders()
{
    // These are the columns for the devices table
    QStringList headers = {"ID", "Status", "Name", "VID:PID", "Serial", "Port", "Interfaces"};
    setHeaders(headers);
}

QVariant UsbGuardDevicesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return table.at(0).at(section);
    }

    return QVariant();
}

void UsbGuardDevicesModel::loadDeviceData(const QList<QVector<QString>>& devices)
{
    // Notify the view that the data will change
    beginResetModel();

    // Clear all previous device data (keep the header row at index 0)
    while(table.size() > 1)
    {
        table.removeLast();
    }

    // Add the new device data
    for(const auto& deviceRow : devices) {
        table.append(deviceRow);
    }

    // Notify the view that the changes are complete
    endResetModel();
}

void UsbGuardDevicesModel::connectToDBus()
{
    // The USBGuard D-Bus service is on the system bus
    QDBusConnection bus = QDBusConnection::systemBus();


    // Check connection
    if(!bus.isConnected()) {
        qCritical() << "Cannot connect to the system D-Bus: " << bus.lastError().message();
        return;
    }

    // Create the D-Bus Interface Object
    m_dbusInterface = new QDBusInterface(
        "org.usbguard1",             // Service Name
        "/org/usbguard1/Devices",    // Object Path (for devices interface)
        "org.usbguard.Devices1",     // Interface Name
        bus
    );

    // If Interface is not valid report this and delete the interface
    if(!m_dbusInterface->isValid()) {
        qCritical() << "D-Bus Interface to USB Guard is not valid! " << m_dbusInterface->lastError().message();
        delete m_dbusInterface;
        m_dbusInterface = nullptr;
        return;
    }


    // --- FIX: Register the custom D-Bus type ---
    qDBusRegisterMetaType<QList<QVariantList>>();


    // Initial Device List Retrieval (Asynchronous Call)
    QDBusPendingCall asyncCall = m_dbusInterface->asyncCall("listDevices", "match");
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(asyncCall, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
        // Corrected template argument: QList<QVariantList> or QList<QVariant>
        // depending on the exact D-Bus version/API mapping, but QList<QVariantList> is more precise for structured arrays.
        QDBusPendingReply<QList<QVariantList>> reply = *watcher;

        if (reply.isError()) {
            qWarning() << "Error fetching device list:" << reply.error().message();
        } else {
            QList<QVector<QString>> deviceList;

            // The reply.value() is a QList<QVariantList>, where each QVariantList is one device
            for (const QVariantList& deviceData : reply.value()) {

                // For listDevices, the structure is (uint id, QString device_rule)
                if (deviceData.count() < 2) continue; // Skip malformed entries

                uint id = deviceData.at(0).toUInt(); // The device ID
                QString rule = deviceData.at(1).toString(); // The device rule string

                // NOTE: The device rule string needs parsing to get all attributes.
                // For a temporary placeholder:
                QString target = rule.split(' ').value(0, "UNKNOWN");

                deviceList.append({
                    QString::number(id),
                    target.toUpper(),
                    rule, // Using the full rule string as a temporary description
                    "...",
                    "...",
                    "...",
                    "..."
                });
            }

            if (!deviceList.isEmpty()) {
                // Call your device loading function here
                loadDeviceData(deviceList);
                qDebug() << "Successfully loaded" << deviceList.size() << "devices.";
            }
        }
        watcher->deleteLater();
    });

    // // 3. Connect to the real-time D-Bus signal
    // bus.connect(
    //     "org.usbguard.Policy1",
    //     "/org/usbguard/Devices",
    //     "org.usbguard.Devices1",
    //     "DevicePresenceChanged",
    //     this,
    //     SLOT(devicePresenceChanged(uint, uint, QString, QString, const QVariantMap&))
    // );
}

void UsbGuardDevicesModel::devicePresenceChanged(
    uint id,
    uint event,
    QString target,
    QString device_rule,
    const QVariantMap &attributes)
{

}
