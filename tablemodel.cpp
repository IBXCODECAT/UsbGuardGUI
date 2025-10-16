#include "tablemodel.h"

TableModel::TableModel(QObject *parent) : QAbstractTableModel(parent)
{
    table.append({"ID", "Status", "Name", "VID:PID", "Serial", "Port", "Interfaces"});

    table.append({"0", "Allowed", "Test", "VID:PID", "Serial", "Port", "Interfaces"});
    table.append({"1", "Blocked", "Test", "VID:PID", "Serial", "Port", "Interfaces"});
}

int TableModel::rowCount(const QModelIndex &) const { return table.size(); }

int TableModel::columnCount(const QModelIndex &) const { return table[0].size(); }

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    switch(role)
    {
    case TableDataRole:
        return table.at(index.row()).at(index.column());
    case HeaderRole:
        if(index.row() == 0) return true;
        return false;
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> TableModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TableDataRole] = "tabledata";
    roles[HeaderRole] = "header";
    return roles;
}
