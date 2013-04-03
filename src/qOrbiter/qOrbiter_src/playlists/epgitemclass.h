#ifndef EPGITEMCLASS_H
#define EPGITEMCLASS_H

#include <QAbstractListModel>
#include <QObject>
#include <QVariant>
#include <QMap>
#include <QImage>

class EPGItemClass : public QObject
{
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::DisplayRole+1,
        IndexRole =Qt::DisplayRole+2,
        ChannelRole= Qt::DisplayRole+3,
        ProgramRole = Qt::DisplayRole+5,
        IdRole = Qt::DisplayRole+6,
        ChanImageRole = Qt::DisplayRole+7,
        ProgImageRole = Qt::DisplayRole+8,
        ChannelIdRole = Qt::DisplayRole+9
    };

public:
    EPGItemClass() {}
    explicit EPGItemClass( QString chanName, int chanIndex,  QString channel, QString program, int dceIndex, QString chanImage, QString progImag, QObject *parent = 0);
    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;

    inline QString name() const { return m_channame; }
    inline int index() const { return m_dceIndex; }
    inline int channel() const { return channel_number; }
    inline QString program() const { return m_program; }
    inline int id() const {  return m_dceIndex; }
    inline QString channelImage() const { return channel_image; }
    inline QString programImage() const {return program_image;}
    inline QString mythid () const {return m_channel;}
    inline QMap <QString*, int> attributes() const {return m_mapAttrib;}
private:
    QString m_channame;
    int m_chanindex;
    QString m_channel;
    QString m_program;
    int channel_number;
    int m_dceIndex;
    QString channel_image;
    QString program_image;

    QMap <QString*, int> m_mapAttrib;


signals:
    void imageChanged();
    void dataChanged();

public slots:
    void destruct(){
        this->deleteLater();
    }

};

#endif // EPGITEMCLASS_H
