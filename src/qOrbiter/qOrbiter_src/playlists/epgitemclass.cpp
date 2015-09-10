#include "epgitemclass.h"
#include "qdebug.h"

EPGItemClass::EPGItemClass( QObject *parent ) : GenericModelItem(parent)
{
    QHash<int, QByteArray> names;
    names[NameRole] = "name";                 /** Maps to channel name */
    names[ProgramRole] = "program";           /** Maps to program on channel */
    names[ChannelRole] = "channel";           /** Numeric channel number, user facing */
    names[ChannelIdRole] = "channelid";       /** Numeric number, internal representation. It has the source number prepended to the actual string of digits. Ie. the last three are the tunable digits */
    names[TimeSlotRole] = "timeslot";         /** String representation of the time slot data */
    names[StartTimeRole] = "starttime";         /** String representation of the time slot data */
    names[EndTimeRole] = "endtime";         /** String representation of the time slot data */
    names[ProgramIdRole] = "programid";       /** Experimental */
    names[SeriesIdRole] = "seriesid";         /** Experimental */
    names[BroadcastSourceRole] = "sourceid";  /** The current source being used */
    names[BroadcastSourceNameRole] = "source";/** The Source name */
    names[IdRole] = "id";
     setRoleNames(names);
}


void EPGItemClass::setEpgItemData(QString channelName, QString programName, QString channelNumber,
                                  QString channelId, QString timeSlot, int startTime, int endTime,
                                  QString programId, QString seriesId, QString sourceId, QString sourceName)
{

    m_data.insert(NameRole, channelName);
    m_data.insert(ProgramRole, programName);
    m_data.insert(ChannelRole, channelNumber);
    m_data.insert(ChannelIdRole, channelId);
    m_data.insert(TimeSlotRole, timeSlot);
    m_data.insert(StartTimeRole, startTime);
    m_data.insert(EndTimeRole, endTime);
    m_data.insert(ProgramIdRole, programId);
    m_data.insert(SeriesIdRole, seriesId);
    m_data.insert(BroadcastSourceNameRole, sourceName);
    m_data.insert(BroadcastSourceRole, sourceId);
    m_data.insert(IdRole,channelNumber);
}

bool EPGItemClass::setData(int role, const QVariant &value)
{
  //    LoggerWrapper::GetInstance()->Write(LV_DEBUG, "GenericModelItem.setData");
    if(!m_data.contains(role))
        return false;

    m_data.insert(role, value);
    emit dataChanged();
    return true;
}
