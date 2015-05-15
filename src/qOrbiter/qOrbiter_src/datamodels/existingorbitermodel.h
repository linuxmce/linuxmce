#ifndef EXISTINGORBITERMODEL_H
#define EXISTINGORBITERMODEL_H

#include <QAbstractListModel>
#include "contextobjects/existingorbiter.h"

class ExistingOrbiterModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ExistingOrbiterModel(ExistingOrbiter* prototype, QObject *parent = 0);
#ifdef QT5
    QHash<int, QByteArray> roleNames() const;
#endif
    //  int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent) const;
    void appendRows(const QList<ExistingOrbiter*> &items);
    void insertRow(int row, ExistingOrbiter* item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    // bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    QModelIndex indexFromItem( const ExistingOrbiter* item) const;
signals:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

public slots:
    void appendRow(ExistingOrbiter * item);
    void handleItemChange();
    void clear() { m_list.clear();}
    int count (){return m_list.length();}

private:
    ExistingOrbiter *m_prototype;
    QList<ExistingOrbiter*>m_list;
    
};

#endif // EXISTINGORBITERMODEL_H
