#include "dbususbproxy.h"

DBusUsbProxy::DBusUsbProxy() : m_sysBus(QDBusConnection::systemBus()) {


    if(m_sysBus.isConnected()) {
        qDebug() << "System bus connected!";
    } else {
        qCritical() << "Failed to connect to system D-Bus: " << m_sysBus.lastError().message();
        return; // Early exit if connection failed
    }

    // Create the D-Bus Interface Object
    QDBusInterface* interfacePtr = new QDBusInterface(
        "org.usbguard1",             // Service Name
        "/org/usbguard1/Devices",    // Object Path (for devices interface)
        "org.usbguard.Devices1",     // Interface Name
        m_sysBus
    );

    // Interface Validity Checks
    if(interfacePtr->isValid()) {
        m_usbInterface.reset(interfacePtr);
        qDebug() << "USBGuard D-Bus interface created successfully.";
    } else {
        qCritical() << "Failed to create USBGuard D-Bus Interface: " << interfacePtr->lastError().message();
        delete interfacePtr;
    }
}

bool DBusUsbProxy::IsConnected() {
    return m_sysBus.isConnected();
}

bool DBusUsbProxy::IsInterfaceValid() {
    return m_usbInterface != nullptr && m_usbInterface->isValid();
}

DBusUsbProxy::~DBusUsbProxy() {}
