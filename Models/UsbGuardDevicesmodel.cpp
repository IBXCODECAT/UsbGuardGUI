#include "UsbGuardDevicesmodel.h"

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusReply>

#include <QtDBus/QDBusMetaType>


UsbGuardDevicesModel::UsbGuardDevicesModel(QObject *parent) : TableModelBase(parent)
{
    initializeHeaders();
}

void UsbGuardDevicesModel::initializeHeaders()
{
    // These are the columns for the devices table
    m_headers = {"ID", "Status", "Name", "VID:PID", "Serial", "Port"};
    setHeaders(m_headers);
}

// FIX: This implementation was wrong. It should read from the headers set in initializeHeaders.
// Assuming TableModelBase stores headers in a member variable, e.g., 'm_headers'.
// If not, it needs to be changed to:
QVariant UsbGuardDevicesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        // Assuming setHeaders saves them in a member list, e.g., m_headers or similar
        // If not, this is a placeholder to show the intent:
        if (section >= 0 && section < m_headers.size()) {
            return m_headers.at(section);
        }
    }

    return QVariant();
}

void UsbGuardDevicesModel::loadDeviceData(const QList<UsbDeviceData>& devices)
{
    // Notify the view that the data will change
    beginResetModel();

    // Clear all previous device data (keep the header row at index 0)
    // The previous implementation was: while(table.size() > 1) { table.removeLast(); }
    // which assumed table[0] was the header. We'll stick to that,
    // but we need to convert UsbDeviceData to the row format.
    while(table.size() > 1)
    {
        table.removeLast();
    }

    // Add the new device data, converting the struct to a row of strings
    for(const auto& device : devices) {
        QVector<QString> deviceRow;
        // The column order is: {"ID", "Status", "Name", "VID:PID", "Serial", "Port"}
        deviceRow.append(QString::number(device.id));
        deviceRow.append(device.status);
        deviceRow.append(device.name);
        deviceRow.append(device.vidPid);
        deviceRow.append(device.serial);
        deviceRow.append(device.port); // 'port' is empty in your parser, but kept for column consistency
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

void UsbGuardDevicesModel::devicePresenceChanged(
    uint id,
    uint event,
    QString target,
    QString device_rule,
    const QVariantMap &attributes)
{

}
