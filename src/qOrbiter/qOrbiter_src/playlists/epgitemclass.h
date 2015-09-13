#ifndef EPGITEMCLASS_H
#define EPGITEMCLASS_H

#include <QAbstractListModel>
#include <QObject>
#include <QVariant>
#include <QMap>
#include <QImage>
#include <datamodels/genericmodelitem.h>

class EPGItemClass : public GenericModelItem
{
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::DisplayRole+1,
        IdRole = Qt::DisplayRole+2,
        IndexRole =Qt::DisplayRole+3,
        ChannelRole= Qt::DisplayRole+4,
        ChannelIdRole = Qt::DisplayRole+5,
        ProgramRole = Qt::DisplayRole+6,
        ProgramIdRole           =Qt::DisplayRole+7,
        SeriesIdRole            =Qt::DisplayRole+8,
        TimeSlotRole=Qt::DisplayRole+9,
        StartTimeRole=Qt::DisplayRole+10,
        EndTimeRole=Qt::DisplayRole+11,
        BroadcastSourceRole     =Qt::DisplayRole+12,
        BroadcastSourceNameRole =Qt::DisplayRole+13,
        ChanImageRole = Qt::DisplayRole+14,
        ProgImageRole = Qt::DisplayRole+15,
        SynopsisRole = Qt::DisplayRole+16,
        RecordingRole = Qt::DisplayRole+17,
        RecordIDRole = Qt::DisplayRole+18,

    };

public:
    EPGItemClass() {}
    void setEpgItemData(QString channelName, QString programName, QString channelNumber, QString channelId,
                        QString timeSlot, int startTime, int endTime,
                        QString programId, QString seriesId, QString sourceId, QString sourceName,
                        QString info, QString recording, QString recordID);
    bool setData(int role, const QVariant &value);
    // explicit EPGItemClass( QString chanName, int chanIndex,  QString channel, QString program, int dceIndex, QString timeSlot, QString seriesId, QString programId, QString broadcastSource, int broadcastSourceIndex, QString chanImage, QString progImag, QObject *parent = 0);
    explicit EPGItemClass(QObject *parent = NULL);

private:
    QString m_channame;
    QString m_channel;
    QString m_channelId;
    int m_chanindex;


    QString m_program;
    QString m_programId;
    QString m_seasonId;

    int channel_number;
    int m_dceIndex;

    QString m_timeSlot;

    int m_broadcastSourceIndex;
    QString m_broadcastSourceName;

    QString channel_image;
    QString program_image;

    QMap <QString*, int> m_mapAttrib;

signals:
    void imageChanged();
    void dataChanged();

public slots:


};

#endif // EPGITEMCLASS_H
