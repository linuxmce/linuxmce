#include "entertainareatimecode.h"
#include "qstringlist.h"

EntertainAreaTimeCode::EntertainAreaTimeCode():
i_eaId(-1), m_eaName("undefined"), m_currentTimeCode("00:00:00.000"), m_totalTimeCode("OO:00:00.000")
{

}

EntertainAreaTimeCode::EntertainAreaTimeCode(QString time, QString totalTime):
    m_currentTimeCode(time),
    m_totalTimeCode(totalTime),
    m_eaName("undefined"),
    i_eaId(-1), i_roomId(-1)
{

}

EntertainAreaTimeCode::EntertainAreaTimeCode(int ea, QString eaName, int room):
    i_eaId(ea),
    m_eaName(eaName),
    i_roomId(room),
    m_currentTimeCode("00:00:00.000"),
    m_totalTimeCode("OO:00:00.000")
{

}

QString EntertainAreaTimeCode::currentTimeCode() const
{
    return m_currentTimeCode;
}

void EntertainAreaTimeCode::setCurrentTimeCode(const QString &currentTimeCode)
{
    if(m_currentTimeCode==currentTimeCode)return;
     m_currentTimeCode = currentTimeCode;

     m_currentTimeCode.remove(".\d\d\d");


     QStringList current = m_currentTimeCode.split(":");
     if(current.length() > 2){
         int hoursToSec = current.at(0).toInt() * 3600 ;
         int minuteToSec = (current.at(1).toInt() * 60);
         int seconds = current.at(2).toInt();
         int currentPosition = hoursToSec + minuteToSec + seconds;
         setTimecodePosition(currentPosition);
     }

    emit currentTimeCodeChanged();

}
QString EntertainAreaTimeCode::totalTimeCode() const
{
    return m_totalTimeCode;
}

void EntertainAreaTimeCode::setTotalTimeCode(const QString &totalTimeCode)
{

   if(m_totalTimeCode == totalTimeCode) return;
    m_totalTimeCode = totalTimeCode;

    QStringList times = totalTimeCode.split(":");

    if(times.length() <= 2){

       // qDebug() << Q_FUNC_INFO << "Time code parse error. Avoided galactic destruction" << times;
        return;
    }

    int hoursToSec = times.at(0).toInt() * 3600 ;
    int minuteToSec = (times.at(1).toInt() * 60);
    int seconds = times.at(2).toInt();

    int totalSeconds = hoursToSec + minuteToSec + seconds;

    setTimecodeLength(totalSeconds);
    emit totalTimeCodeChanged();

}
int EntertainAreaTimeCode::getI_roomId() const
{
    return i_roomId;
}

void EntertainAreaTimeCode::setI_roomId(int value)
{
    i_roomId = value;
}

int EntertainAreaTimeCode::dragTime() const
{
    return m_dragTime;
}

void EntertainAreaTimeCode::setDragTime(int dragTime)
{
    m_dragTime = dragTime;

    emit dragTimeChanged();

    int displayHours = dragTime / (3600);
    int remainder = dragTime % 3600;
    int minutes = remainder / 60;
    int forseconds = remainder % 60;

    setDragString(QString::number(displayHours)+":"+QString::number(minutes)+":"+QString::number(forseconds) );

}

QString EntertainAreaTimeCode::dragString() const
{
    return m_dragString;
}

void EntertainAreaTimeCode::setDragString(const QString &dragString)
{
    m_dragString = dragString;
    emit dragStringChanged();
}

int EntertainAreaTimeCode::timecodePosition() const
{
    return m_timecodePosition;
}

void EntertainAreaTimeCode::setTimecodePosition(int timecodePosition)
{
    if(m_timecodePosition==timecodePosition)return;
    m_timecodePosition = timecodePosition;

}

void EntertainAreaTimeCode::finishDragging(int i_dragTime)
{
     emit seekToTime(QString::number(i_dragTime));
}

int EntertainAreaTimeCode::timecodeLength() const
{
    return m_timecodeLength;
}

void EntertainAreaTimeCode::setTimecodeLength(int timecodeLength)
{
    if(m_timecodeLength==timecodeLength)return;
    m_timecodeLength = timecodeLength;

}


QString EntertainAreaTimeCode::eaName() const
{
    return m_eaName;
}

void EntertainAreaTimeCode::setEaName(const QString &eaName)
{
    m_eaName = eaName;
}

int EntertainAreaTimeCode::eaId() const
{
    return i_eaId;
}

void EntertainAreaTimeCode::setEaId(int value)
{
    i_eaId = value;
}




