#ifndef DBUSUSBPROXY_H
#define DBUSUSBPROXY_H


#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusMetaType>

class DBusUsbProxy
{
public:
    DBusUsbProxy();
    ~DBusUsbProxy();

    inline bool IsConnected();
    inline bool IsInterfaceValid();

private:
    const QDBusConnection m_sysBus;
    std::unique_ptr<QDBusInterface> m_usbInterface = nullptr;
};

#endif // DBUSUSBPROXY_H
