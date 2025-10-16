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

struct UsbGuardDevice
{
    uint id;
    QString rule;
};

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

    qDBusRegisterMetaType<QPair<uint, QString>>();
    qDBusRegisterMetaType<QList<QPair<uint, QString>>>();

    QDBusPendingCall asyncCall = m_dbusInterface->asyncCall("listDevices", "match");
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(asyncCall, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {

        // Use the type that matches the D-Bus signature 'a(us)'
        QDBusPendingReply<QList<QPair<uint, QString>>> reply = *watcher;

        if (reply.isError()) {
            qWarning() << "Error fetching device list:" << reply.error().message();
        } else {
            QList<QVector<QString>> deviceList;

            // Iterate over the correctly typed reply
            for (const QPair<uint, QString>& deviceData : reply.value()) {

                uint id = deviceData.first;         // 'u'
                QString rule = deviceData.second;   // 's'
                // ... rest of your parsing logic ...

                // 1. Determine Status (First token is the target, e.g., 'allow', 'block', 'reject')
                QStringList ruleParts = rule.split(' ', Qt::SkipEmptyParts);
                QString status = ruleParts.value(0, "UNKNOWN").toUpper(); // ALLOW, BLOCK, REJECT, etc.

                // 2. Simple Rule String Parsing (This is an approximation—real parsing is complex)
                // You'll need a robust way to extract key-value pairs from the rule string.
                // For now, let's use placeholders and focus on the ID/Status

                // This is a common pattern for extraction:
                auto extractValue = [&](const QString& key) -> QString {
                    int keyIndex = ruleParts.indexOf(key);
                    if (keyIndex != -1 && keyIndex + 1 < ruleParts.size()) {
                        QString value = ruleParts.at(keyIndex + 1);
                        // Handle quoted strings for 'name'
                        if (value.startsWith('"') && value.endsWith('"')) {
                            return value.mid(1, value.length() - 2);
                        }
                        return value;
                    }
                    return "...";
                };

                QString vidPid = extractValue("id");
                QString serial = extractValue("serial");
                QString port   = extractValue("port");
                QString name   = extractValue("name");
                // Interfaces aren't easily extracted from the simple rule string; placeholder.

                deviceList.append({
                    QString::number(id),
                    status,
                    name.isEmpty() ? rule : name, // Use name if available, otherwise the rule string
                    vidPid,
                    serial,
                    port,
                    "..." // Interfaces
                });
            }

            watcher->deleteLater();
            // ...

            if (!deviceList.isEmpty()) {
                loadDeviceData(deviceList);
                qDebug() << "Successfully loaded" << deviceList.size() << "devices.";
            }
        }

    });
}


void UsbGuardDevicesModel::devicePresenceChanged(
    uint id,
    uint event,
    QString target,
    QString device_rule,
    const QVariantMap &attributes)
{

}
