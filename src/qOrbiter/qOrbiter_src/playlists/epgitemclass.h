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
        NameRole = Qt::UserRole+1,
        IndexRole =Qt::UserRole+2,
        ChannelRole= Qt::DisplayRole+3,
        ProgramRole = Qt::UserRole+5,
        IdRole = Qt::DisplayRole+6,
        ChanImageRole = Qt::DisplayRole+7,
        ProgImageRole = Qt::DisplayRole+8,
        ChannelIdRole = Qt::DisplayRole+9
    };

public:
    EPGItemClass(QObject *parent = 0) {}
    explicit EPGItemClass( QString &chanName, int &chanIndex,  QString &channel, QString &program, int &dceIndex, QImage &chanImage, QImage &progImag,  QObject *parent = 0);
    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;

    inline QString name() const { return m_channame; }
    inline int index() const { return m_dceIndex; }
    inline int channel() const { return channel_number; }
    inline QString program() const { return m_program; }
    inline int id() const {  return m_dceIndex; }
    inline QImage channelImage() const { return channel_image; }
    inline QImage programImage() const {return program_image;}
    inline QString mythid () const {return m_channel;}


    inline QMap <QString*, int> attributes() const {return m_mapAttrib;}
private:
    QString m_channame;
    int m_chanindex;
    QString m_channel;
    QString m_program;
    int channel_number;
    int m_dceIndex;
    QImage channel_image;
    QImage program_image;

    QMap <QString*, int> m_mapAttrib;


signals:
    void imageChanged();
    void dataChanged();

};

#endif // EPGITEMCLASS_H
