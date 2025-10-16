#ifndef TABLEMODELBASE_H
#define TABLEMODELBASE_H

#include <QObject>
#include <QVector>
#include <QAbstractTableModel>

class TableModelBase : public QAbstractTableModel
{
    Q_OBJECT
protected:
    enum TableRoles {
        TableDataRole = Qt::UserRole + 1,
        HeaderRole
    };

public:
    explicit TableModelBase(QObject *parent = nullptr);

    int rowCount(const QModelIndex & = QModelIndex()) const override;
    int columnCount(const QModelIndex & = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QHash<int, QByteArray> roleNames() const override;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override = 0;

protected:
    QVector<QVector<QString>> table;

    void setHeaders(const QStringList& headers);

signals:
public slots:
};

#endif // TABLEMODELBASE_H
