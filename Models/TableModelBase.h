#ifndef TABLEMODELBASE_H
#define TABLEMODELBASE_H

#include <QAbstractTableModel>
#include <QVector>
#include <QString>
#include <QHash>
#include <QByteArray>

// Custom roles for QML/QVariant interaction
enum TableRoles {
    TableDataRole = Qt::UserRole + 1, // The actual data string
    HeaderRole // A boolean role, true if it's the header row
};

class TableModelBase : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TableModelBase(QObject *parent = nullptr);

    // QAbstractTableModel overrides
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    // NOTE: headerData is *pure virtual* in QAbstractTableModel, so it must be
    // implemented in the derived class (UsbGuardDevicesModel), not here.

    // Custom role name mapping for QML
    QHash<int, QByteArray> roleNames() const override;

protected:
    // Internal method to set headers, which also clears data
    void setHeaders(const QStringList& headers);

    // Internal storage for the table data, including the header row at index 0.
    // Each inner QVector<QString> represents a row.
    QVector<QVector<QString>> table;
};

#endif // TABLEMODELBASE_H
