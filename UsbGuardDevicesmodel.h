#ifndef USBGUARDDEVICESMODEL_H
#define USBGUARDDEVICESMODEL_H

#include "TableModelBase.h"
#include "DataStructures.h"
#include <QList>
#include <QStringList>
#include <QVariantMap>

// Note: UsbGuardDevicesModel is a better name than UsbGuardDataModel

class UsbGuardDevicesModel : public TableModelBase
{
    Q_OBJECT
    // Expose this model to QML with a unique name
    Q_PROPERTY(QStringList headers READ headers CONSTANT)

public:
    explicit UsbGuardDevicesModel(QObject *parent = nullptr);

    // QAbstractTableModel/TableModelBase overrides
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Public method to receive data from UsbGuardManager
    void loadDeviceData(const QList<UsbDeviceData>& devices);

    // D-Bus signal handler (placeholder in your original code)
    void devicePresenceChanged(
        uint id,
        uint event,
        QString target,
        QString device_rule,
        const QVariantMap &attributes);

    QStringList headers() const { return m_headers; }

private:
    void initializeHeaders();

    // Member to store the column headers, used for headerData()
    QStringList m_headers;

    // The struct UsbGuardDevice from the implementation should be here
    // as it's a specific type used by the model's signal handler.
    struct UsbGuardDevice
    {
        uint id;
        QString rule;
    };
};

#endif // USBGUARDDEVICESMODEL_H
