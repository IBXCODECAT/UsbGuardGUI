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
    QStringList headers = {"ID", "Status", "Name", "VID:PID", "Serial", "Port"};
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

void UsbGuardDevicesModel::devicePresenceChanged(
    uint id,
    uint event,
    QString target,
    QString device_rule,
    const QVariantMap &attributes)
{

}
