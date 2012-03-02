

#ifndef EPGCHANNELLIST_H
#define EPGCHANNELLIST_H


#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <playlists/epgitemclass.h>
#include <qorbitermanager.h>


class qorbiterManager;


class EPGChannelList : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit EPGChannelList(EPGItemClass* prototype);
    ~EPGChannelList();

    void clear();
    bool isActive;

    bool checkDupe(QString name, QString position);
    void setItemStatus(int i);

public slots:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void appendRow(EPGItemClass* item);
    void appendRows(const QList<EPGItemClass*> &items);
    void insertRow(int row, EPGItemClass* item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    EPGItemClass* takeRow(int row);
    EPGItemClass* find(const QString &id) const;
    //Q_INVOKABLE QVariant get(int index, const QString &name) const;
    QModelIndex indexFromItem( const EPGItemClass* item) const;
    EPGItemClass* currentRow();
    void populate();

    QModelIndex getChannelIndex(const QString &name) const;

signals:
    void ItemAdded();
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const int &sRow);
    void requestEpg();

private slots:
    void handleItemChange();

private:
    EPGItemClass* m_prototype;
    QList<EPGItemClass*> m_list;

};

#endif // EPGCHANNELLIST_H
