#ifndef EPGCHANNELLIST_H
#define EPGCHANNELLIST_H

#include <QAbstractListModel>
#include <playlists/epgitemclass.h>
#include <QVariant>
#include <QList>

class EPGChannelList : public QAbstractListModel
{
public:
    explicit EPGChannelList(EPGItemClass* prototype, QObject* parent = 0);
    ~EPGChannelList();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void appendRow(EPGItemClass* item);
    void appendRows(const QList<EPGItemClass*> &items);
    void insertRow(int row, EPGItemClass* item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    EPGItemClass* takeRow(int row);
    EPGItemClass* find(const QString &id) const;
    QModelIndex indexFromItem( const EPGItemClass* item) const;
    EPGItemClass* currentRow();
    bool checkDupe(QString name, QString position);
    void setItemStatus(int b);
    void clear();
    bool isActive;

public  slots:

signals:
    void ItemAdded();
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const int &sRow);

private slots:
  void handleItemChange();

private:
    EPGItemClass* m_prototype;
    QList<EPGItemClass*> m_list;

};

#endif // EPGCHANNELLIST_H
