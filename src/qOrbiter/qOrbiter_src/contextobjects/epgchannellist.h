/*
    This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org
   Langston Ball  golgoj4@gmail.com
    QOrbiter is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QOrbiter is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QOrbiter.  If not, see <http://www.gnu.org/licenses/>.

    EPGChannlist.h/cpp is designed as a dynamic listmodel c++ object intended to be consumed by a qml listmodel. Its purpose is to hold the epg data
    for one time block, provide metadata when the user changes channels and its own internal cleanup and index management. This class is used by both
    MythTv and liveTv.
*/

#ifndef EPGCHANNELLIST_H
#define EPGCHANNELLIST_H


#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <playlists/epgitemclass.h>
#include <qorbitermanager.h>
#ifdef QT5
#include <QtWidgets/QApplication>
#endif

class qorbiterManager;


class EPGChannelList : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QModelIndex activeIndex READ getCurrentIndex WRITE setCurrentIndex NOTIFY activeIndexChanged)
    Q_PROPERTY(int currentIndex READ getCurrentRow WRITE setCurrentRow NOTIFY activeRowChanged)
    Q_PROPERTY(double progress READ getProgress WRITE setProgress NOTIFY progressChanged)
public:
    explicit EPGChannelList(EPGItemClass* prototype);
    ~EPGChannelList();

#ifdef QT5
    QHash <int, QByteArray> roleNames() const ;
#endif

    bool isActive;
 //nowPlayingButton->setProgram(simpleEPGmodel->data(simpleEPGmodel->getChannelIndex(chanid), 5).toString());
    bool checkDupe(QString name, QString position);
    void setItemStatus(int i);
    QModelIndex activeIndex;
    int currentIndex;
    QString id;
    double progress;

public slots:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void appendRow(EPGItemClass *item);
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
    void updateLivePosition();
    void updatePosition();
    QModelIndex getCurrentIndex();
    QModelIndex getMythChannelIndex(QString m);
    void empty();
    void sortModel(int column, Qt::SortOrder order);

    void setProgress(double d){progress=d; emit progressChanged(); }
    double getProgress() {return progress;}


    void setCurrentRow(int row) {currentIndex = row; emit activeRowChanged(); }
    int getCurrentRow() {return currentIndex;}

    QModelIndex getChannelIndex(const QString &name) const;

    void itemDeleted(){
   //     qDebug() << "Model Item Destroyed Signal emitted.";
    }
void clear();
    //void clear() { m_list.clear(); }

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
    void progressChanged();

private slots:
    void handleItemChange();

private:
    EPGItemClass* m_prototype;
    QList<EPGItemClass*> m_list;
    void resetInternalData();

};

#endif // EPGCHANNELLIST_H
