#include "DBusUsbProxy.h"
#include <QtDBus/QDBusPendingCallWatcher>

// Register the custom types required for the D-Bus communication
static bool typeRegistration = []() {
    qDBusRegisterMetaType<QPair<uint, QString>>();
    qDBusRegisterMetaType<QList<QPair<uint, QString>>>();
    qDBusRegisterMetaType<QMap<QString, QString>>(); // For D-Bus a{ss}
    return true;
}();

DBusUsbProxy::DBusUsbProxy(QObject *parent) : m_sysBus(QDBusConnection::systemBus()) {

    if(m_sysBus.isConnected()) {
        qDebug() << "System bus connected!";
    } else {
        qCritical() << "Failed to connect to system D-Bus: " << m_sysBus.lastError().message();
        return; // Early exit if connection failed
    }

    // Create the D-Bus Interface Object
    QDBusInterface* rawInterfacePtr = new QDBusInterface(
        "org.usbguard1",             // Service Name
        "/org/usbguard1/Devices",    // Object Path (for devices interface)
        "org.usbguard.Devices1",     // Interface Name
        m_sysBus
    );

    // Interface Validity Checks
    if(rawInterfacePtr->isValid()) {
        m_usbInterface.reset(rawInterfacePtr);
        qDebug() << "USBGuard D-Bus interface created successfully.";

        // Connect to the DevicePolicyChanged signal on the system bus.
        m_sysBus.connect(
            "org.usbguard1",           // Service Name
            "/org/usbguard1/Devices",  // Object Path
            "org.usbguard.Devices1",   // Interface Name
            "DevicePolicyChanged",     // Signal Name
            "uuusua{ss}",              // D-Bus Type Signature
            this,
            SLOT(handleRemotePolicyChanged(uint, uint, uint, QString, uint, QMap<QString, QString>))
            );

        qDebug() << "DevicePolicyChanged signal connection established."; // More accurate message

    } else {
        qCritical() << "Failed to create USBGuard D-Bus Interface: " << rawInterfacePtr->lastError().message();
        delete rawInterfacePtr;
    }

}

// ----------------------------------------------------------------------
// Public Method for Client (GUI) Interaction
// ----------------------------------------------------------------------
void DBusUsbProxy::listDevices()
{
    if (!IsInterfaceValid()) {
        emit errorOccurred("D-Bus interface is not valid. Cannot list devices.");
        return;
    }

    // Call "listDevices" with the argument "match" asynchronously
    QDBusPendingCall asyncCall = m_usbInterface->asyncCall("listDevices", "match");

    // Create the watcher and parent it to 'this' to manage its lifetime
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(asyncCall, this);

    // Connect the watcher's finished signal to our private reply slot
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &DBusUsbProxy::handleListDevicesReply);

    qDebug() << "ListDevices D-Bus call initiated.";
}

// ----------------------------------------------------------------------
// Private Slots (Handling D-Bus Asynchronous Replies and Signals)
// ----------------------------------------------------------------------
void DBusUsbProxy::handleListDevicesReply(QDBusPendingCallWatcher *const watcher)
{
    // The watcher is a QObject created with 'new', delete it gracefully
    watcher->deleteLater();

    // The type must match the D-Bus signature 'a(us)'
    QDBusPendingReply<QList<QPair<uint, QString>>> reply = *watcher;

    if (reply.isError()) {
        QString errorMsg = QString("Error fetching device list: %1").arg(reply.error().message());
        qWarning() << errorMsg;
        emit errorOccurred(errorMsg); // Notify the GUI
        return;
    }

    // Delegate the complex parsing to the helper function
    QList<UsbDeviceData> deviceList = parseRuleStrings(reply.value());

    qDebug() << "Successfully retrieved and parsed" << deviceList.size() << "devices.";
    emit deviceListReady(deviceList); // Notify the GUI with the clean data
}

void DBusUsbProxy::handleRemotePolicyChanged(
    uint id,
    uint target_old,
    uint target_new,
    const QString& device_rule,
    uint rule_id,
    const QMap<QString, QString> &attributes) // <-- FIX: CHANGED TO QMap<QString, QString>
{
    qDebug() << "D-Bus Signal: DevicePolicyChanged received."
             << "ID:" << id << "Target Old:" << target_old
             << "Target New:" << target_new
             << "Rule:" << device_rule
             << "Attributes Count:" << attributes.size();

    // NO CONVERSION NEEDED NOW! The data is already QMap<QString, QString>

    emit deviceListChanged();
}

// ----------------------------------------------------------------------
// Private Helper Function (Data Translation)
// ----------------------------------------------------------------------

QList<UsbDeviceData> DBusUsbProxy::parseRuleStrings(const QList<QPair<uint, QString>>& rawData)
{
    QList<UsbDeviceData> deviceList;

    for (const QPair<uint, QString>& deviceData : rawData) {

        UsbDeviceData device;
        device.id = deviceData.first; // The device ID ('u' in the D-Bus signature)
        const QString& rule = deviceData.second; // The rule string ('s' in the D-Bus signature)

        // Split the rule string into tokens for parsing
        QStringList ruleParts = rule.split(' ', Qt::SkipEmptyParts);

        // Lambda function to safely extract a value associated with a key token
        auto extractValue = [&](const QString& key) -> QString {
            int keyIndex = ruleParts.indexOf(key);
            if (keyIndex != -1 && keyIndex + 1 < ruleParts.size()) {
                QString value = ruleParts.at(keyIndex + 1);
                // Handle quoted strings (e.g., name "My Device")
                if (value.startsWith('"') && value.endsWith('"')) {
                    return value.mid(1, value.length() - 2);
                }
                return value;
            }
            return QString();
        };

        // 1. Status (The first token is the policy action)
        device.status = ruleParts.value(0, "UNKNOWN").toUpper();

        // 2. Device Attributes Extraction
        device.vidPid = extractValue("id");
        device.serial = extractValue("serial");
        QString nameFromRule = extractValue("name");

        // 3. Name fallback
        device.name = nameFromRule.isEmpty() ? rule : nameFromRule;

        deviceList.append(device);
    }

    return deviceList;
}
