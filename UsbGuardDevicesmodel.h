#ifndef USBGUARDDEVICESMODEL_H
#define USBGUARDDEVICESMODEL_H

#include <QtDBus/QDBusInterface>
#include "TableModelBase.h"

class UsbGuardDevicesModel : public TableModelBase
{
    Q_OBJECT
public:
    explicit UsbGuardDevicesModel(QObject *parent = nullptr);

    // Override the pure virtual function from the base class
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE void loadDeviceData(const QList<QVector<QString>>& devices);

public slots:
    // Slot to handle real-time device connection/disconnection/update events
    void devicePresenceChanged(uint id, uint event, QString target, QString device_rule, const QVariantMap &attributes);

private:
    void initializeHeaders();

    // D-Bus object to interact with the USBGuard daemon
    QDBusInterface *m_dbusInterface;
    void connectToDBus();
};

#endif // USBGUARDDEVICESMODEL_H
