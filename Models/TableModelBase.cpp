#include "TableModelBase.h"

TableModelBase::TableModelBase(QObject *parent) : QAbstractTableModel(parent)
{
    // The table is initialized empty here. Derived classes will set headers/data.
}

void TableModelBase::setHeaders(const QStringList& headers)
{
    // Clear any existing data
    if (table.size() > 0) {
        beginResetModel();
        table.clear();
        endResetModel();
    }

    // Set the new header row.
    QVector<QString> headerRow;
    for (const QString& h : headers) {
        headerRow.append(h);
    }
    // Only add the header row if the list is not empty
    if (!headerRow.isEmpty()) {
        beginInsertRows(QModelIndex(), 0, 0);
        table.append(headerRow);
        endInsertRows();
    }
}

int TableModelBase::rowCount(const QModelIndex &) const { return table.size(); }

int TableModelBase::columnCount(const QModelIndex &) const
{
    if (table.isEmpty()) return 0; // Prevent crash on empty table
    return table[0].size();
}

QVariant TableModelBase::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= table.size() || index.column() >= columnCount()) {
        return QVariant();
    }

    switch(role)
    {
    case TableDataRole:
        // Use row and column directly; the view will handle if the first row is a header
        return table.at(index.row()).at(index.column());
    case HeaderRole:
        // True for the first row (the header row)
        if(index.row() == 0) return true;
        return false;
    default:
        // Fall through to the derived class or use QAbstractTableModel's default behavior
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> TableModelBase::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TableDataRole] = "tabledata";
    roles[HeaderRole] = "header";
    return roles;
}

// Note: The headerData() function must be implemented in the derived classes since it is a pure virtual method in the base class.
