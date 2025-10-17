#include "DBusUsbProxy.h"
#include <QtDBus/QDBusPendingCallWatcher>

// Register the custom types required for the D-Bus communication
// These are necessary because the USBGuard listDevices method returns
// a list of structs (a(us)), which maps to QList<QPair<uint, QString>> in Qt.
static bool typeRegistration = []() {
    qDBusRegisterMetaType<QPair<uint, QString>>();
    qDBusRegisterMetaType<QList<QPair<uint, QString>>>();
    return true;
}();

Q_DECLARE_METATYPE(QList<UsbDeviceData>) // Register the clean output type for signals

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

        // Connect to D-Bus signals immediately upon interface creation
        // Assumes a signal named "DeviceChanged" exists on the interface
        QObject::connect(m_usbInterface.get(),
                SIGNAL(DeviceChanged(uint, QString)), // D-Bus signal signature
                this,
                SLOT(handleRemoteDeviceChanged(uint, QString)));

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
void DBusUsbProxy::handleListDevicesReply(QDBusPendingCallWatcher *watcher)
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

void DBusUsbProxy::handleRemoteDeviceChanged(uint id, const QString& rule)
{
    qDebug() << "D-Bus Signal: Device Changed received for ID" << id;

    // In a real application, you'd parse the rule here to determine the change type,
    // update your internal device model, and then emit a generic signal.

    // For now, we simply signal the list changed, prompting the GUI to re-request the full list.
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
