#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <QObject>
#include <QString>
#include <QList>
#include <QVariantMap> // Include for the QVariantMap in the original UsbGuardDevice structure (though not used in the final UsbDeviceData)

// ----------------------------------------------------------------------
// Data Structure used for clean data transfer
// This structure holds the parsed data for a single USB device.
// ----------------------------------------------------------------------
struct UsbDeviceData
{
    uint id;
    QString status;
    QString name;
    QString vidPid;
    QString serial;
    QString port;
};

// ----------------------------------------------------------------------
// Qt Metatype Registration
// This is essential for passing the struct and its list counterpart
// across signal/slot connections (especially queued/cross-thread),
// and when storing them in QVariant.
// ----------------------------------------------------------------------
Q_DECLARE_METATYPE(UsbDeviceData)
Q_DECLARE_METATYPE(QList<UsbDeviceData>)

// Note: Although Q_DECLARE_METATYPE is enough, sometimes it's good practice
// to also perform a qRegisterMetaType call if you expect queued connections
// to ensure the type is known immediately. We can place that in the
// UsbGuardManager or main.cpp if needed, but the declarations suffice for
// definition.

#endif // DATASTRUCTURES_H
