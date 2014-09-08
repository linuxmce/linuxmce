#include "PlutoUtils/StringUtils.h"

#include "XineMediaInfo.h"

string XineMediaInfo::ToString()
{
    string sResult;
    sResult += StringUtils::itos(m_iSpeed);
    sResult += ",";
    sResult += FormatCurrentTime();
    sResult += ",";
    sResult += FormatTotalTime();
    sResult += ",";
    sResult += StringUtils::itos(m_iStreamID);
    sResult += ",";
    sResult += StringUtils::itos(m_iTitle);
    sResult += ",";
    sResult += StringUtils::itos(m_iChapter);
    sResult += ",";
    sResult += m_sMediaType;
    sResult += ",";
    sResult += StringUtils::itos(m_iMediaID);
    sResult += ",";
    sResult += m_sFileName;

    return sResult;
}

bool XineMediaInfo::FromString(string sSource)
{
    vector<string> vData = StringUtils::Split(sSource, ",");
    if ( vData.size() != 9 )
        return false;
    
    XineMediaInfo newInfo;
    newInfo.m_iSpeed = atoi(vData[0].c_str());
    if (!ParseTimeString(vData[1], newInfo.m_iPositionInMilliseconds))
        return false;

    if (!ParseTimeString(vData[2], newInfo.m_iTotalLengthInMilliseconds))
        return false;
    
    newInfo.m_iStreamID = atoi(vData[3].c_str());
    newInfo.m_iTitle = atoi(vData[4].c_str());
    newInfo.m_iChapter = atoi(vData[5].c_str());
    newInfo.m_sMediaType = vData[6];
    newInfo.m_iMediaID = atoi(vData[7].c_str());
    newInfo.m_sFileName = vData[8];
    
    *this = newInfo;
    
    return true;
}

string XineMediaInfo::FormatCurrentTime(bool bShowMilliseconds/*=true*/)
{
    return FormatTime(m_iPositionInMilliseconds, bShowMilliseconds);
}

string XineMediaInfo::FormatTotalTime(bool bShowMilliseconds/*=true*/)
{
    return FormatTime(m_iTotalLengthInMilliseconds, bShowMilliseconds);
}

XineMediaInfo::XineMediaInfo()
{
    m_iSpeed = 0;
    m_iPositionInMilliseconds = 0;
    m_iTotalLengthInMilliseconds = 0;
    m_iStreamID = 0;
    m_iTitle = -1;
    m_iChapter = -1;
    m_iMediaID = -1;
    m_sMediaType = "N";
}

string XineMediaInfo::FormatTime(int iTime, bool bShowMilliseconds)
{
    const int iMaxSize = 32;
    char buf[iMaxSize];
    
    if (bShowMilliseconds)
        snprintf(buf, iMaxSize, "%02i:%02i:%02i.%03i",  iTime / 1000 / 3600, iTime / 1000 % 3600 / 60, iTime / 1000 % 3600 % 60, iTime % 1000);
    else
        snprintf(buf, iMaxSize, "%02i:%02i:%02i",  iTime / 1000 / 3600, iTime / 1000 % 3600 / 60, iTime / 1000 % 3600 % 60);
    
    return buf;
}

// NOTE it will parse only "hh:mm:ss.msc", not "hh:mm:ss"
bool XineMediaInfo::ParseTimeString(string sTime, int& iTime)
{
    vector<string> vCurrTime = StringUtils::Split(sTime, ":.");
    if (vCurrTime.size() != 4)
        return false;
    
    iTime = ( atoi(vCurrTime[0].c_str())*3600 + atoi(vCurrTime[1].c_str())*60 + atoi(vCurrTime[2].c_str()) )*1000 + atoi(vCurrTime[3].c_str());
    return true;
}
