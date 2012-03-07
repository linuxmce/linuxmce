

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
    Q_PROPERTY(QModelIndex activeIndex READ getCurrentIndex WRITE setCurrentIndex NOTIFY activeIndexChanged)
    Q_PROPERTY(int currentIndex READ getCurrentRow WRITE setCurrentRow NOTIFY activeRowChanged)

public:
    explicit EPGChannelList(EPGItemClass* prototype);
    ~EPGChannelList();

    void clear();
    bool isActive;
 //nowPlayingButton->setProgram(simpleEPGmodel->data(simpleEPGmodel->getChannelIndex(chanid), 5).toString());
    bool checkDupe(QString name, QString position);
    void setItemStatus(int i);
    QModelIndex activeIndex;
    int currentIndex;
    QString id;

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
    void setProgram(QString qml_text_channel);
    void setCurrentIndex(QModelIndex index);
    void setMythProgram(QString channel);
    void updatePosition();
    QModelIndex getCurrentIndex();
    QModelIndex getMythChannelIndex(QString m);


    virtual void beginResetModel();
    virtual void endResetModel();

    void setCurrentRow(int row) {currentIndex = row; emit activeRowChanged(); }
    int getCurrentRow() {return currentIndex;}

    QModelIndex getChannelIndex(const QString &name) const;

signals:
    void ItemAdded();
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const int &sRow);
    void requestEpg();
    void activeIndexChanged();
    void activeRowChanged();
    void channelNumberChanged(QString chan);
    void programChanged(QString prog);
    void networkChanged(QString net);
    void modelAboutToBeReset();
    void modelReset();

private slots:
    void handleItemChange();

private:
    EPGItemClass* m_prototype;
    QList<EPGItemClass*> m_list;
    void resetInternalData();

};

#endif // EPGCHANNELLIST_H
