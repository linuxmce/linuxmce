#ifndef GENERICSETUPMODEL_H
#define GENERICSETUPMODEL_H
#include <QObject>
#include <QAbstractListModel>
#include <genericesetupitem.h>

class GenericSetupModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit GenericSetupModel(QObject *parent = 0);
    Q_OBJECT
public:

    explicit GenericSetupModel(GenericSetupItem *prototype, QObject* parent = 0);
    ~GenericSetupModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void appendRow(GenericSetupItem* item);
    void appendRows(const QList<GenericSetupItem*> &items);
    void insertRow(int row, GenericSetupItem* item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    GenericSetupItem* takeRow(int row);
    GenericSetupItem* find(const QString &id) const;
    QModelIndex indexFromItem( const GenericSetupItem* item) const;
    void clear();
signals:

public slots:

};

#endif // GENERICSETUPMODEL_H
