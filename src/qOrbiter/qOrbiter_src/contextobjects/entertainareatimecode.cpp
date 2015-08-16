#include "entertainareatimecode.h"

EntertainAreaTimeCode::EntertainAreaTimeCode():
i_eaId(-1), m_eaName("undefined"), m_currentTimeCode("00:00:00.000"), m_totalTimeCode("OO:00:00.000")
{

}

EntertainAreaTimeCode::EntertainAreaTimeCode(QString time, QString totalTime):
    m_currentTimeCode(time),
    m_totalTimeCode(totalTime),
    m_eaName("undefined"),
    i_eaId(-1)
{

}

EntertainAreaTimeCode::EntertainAreaTimeCode(int ea, QString eaName):
    i_eaId(ea),
    m_eaName(eaName),
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
    m_currentTimeCode = currentTimeCode;
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
    emit totalTimeCodeChanged();

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




