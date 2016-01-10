/**
 *
 * @file MythTvWrapper.cpp
 * @brief source file for the MythTvWrapper class
 *
 */
#ifndef WIN32

#include "MythTV_PlugIn.h"
#include "MythTvWrapper.h"

#include "DCE/Logger.h"
#include "Gen_Devices/AllCommandsRequests.h"

#include <qapplication.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlresult.h>
#include <qmutex.h>

#include <libmyth/mythcontext.h>
#include <libmyth/mythdialogs.h>
#include <libmythtv/programinfo.h>

#include <libmythtv/remoteutil.h>

using namespace DCE;

MythContext *gContext;


MythTvWrapper::MythTvWrapper(Command_Impl *pCommandImpl)
    :m_pDCEDeviceWrapper(pCommandImpl)
{
    int argc = 1;
    char *argv[] = { "", "", "" };

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Passing params: %d", argc);

    m_pQApplication = new QApplication(argc, argv, false); /** we don't need to display anything on X. */

    if ( ! initMythTVGlobalContext() )
    {
        delete m_pQApplication;
        throw "Could not init Myth Global Context";
    }
}

MythTvWrapper::~MythTvWrapper()
{
	delete m_pQApplication;
}

bool MythTvWrapper::initMythTVGlobalContext()
{
    gContext = new MythContext(MYTH_BINARY_VERSION); /** we don't need X in the plugin */

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "MythTvWrapper::initMythTVGlobalContext() MythContext object was created.");
    if (!gContext->Init(false))
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "MythTvWrapper::initMythTVGlobalContext() Could not init the MythContext object.");
        return false;
    }

    return true;
}

MythTvWrapper::MythTvEpgGrid::MythTvEpgGrid()
{

}

void MythTvWrapper::MythTvEpgGrid::setGridBoundaries(QDateTime startTime, QDateTime endTime)
{
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Setting grid boundaries to: [%s -> %s]",
                startTime.toString().ascii(),
                endTime.toString().ascii());

    m_timeFrom = startTime;
    m_timeTo   = endTime;

    m_iQuantInSecs = 5 * 60; // 15 minutes;
    m_iColumns = m_timeFrom.secsTo(m_timeTo) / m_iQuantInSecs;

    readChannelData();

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Total grid size %dx%d", GetRows(), GetCols());
}

int MythTvWrapper::MythTvEpgGrid::GetRows()
{
    return m_vectChannelIds.size();
}

int MythTvWrapper::MythTvEpgGrid::GetCols()
{
    return m_iColumns;
}

void MythTvWrapper::MythTvEpgGrid::readChannelData()
{
    QString strQuery, strTimeFormat;

    QString startDataAsString, endDateAsString;

    strTimeFormat = "yyyy-MM-dd hh:mm:ss";

    startDataAsString = m_timeFrom.toString(strTimeFormat);
    endDateAsString   = m_timeTo.toString(strTimeFormat);

    strQuery.sprintf("SELECT p.chanid"
                     " FROM program p"
                     " WHERE p.starttime BETWEEN '%s' and '%s' OR "
                           "p.endtime BETWEEN '%s' and '%s' OR "
                           "(p.starttime <= '%s' AND p.endtime >= '%s')"
                     " GROUP BY p.chanid",
                     startDataAsString.ascii(),
                     endDateAsString.ascii(),
                     startDataAsString.ascii(),
                     endDateAsString.ascii(),
                     startDataAsString.ascii(),
                     endDateAsString.ascii());

    QSqlQuery query;
    query.exec(strQuery);

    if ( ! query.isActive() )
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING,
            "The query %s for the channel count failed: %s!",
            strQuery.ascii(),
            query.lastError().text().ascii());

        return;
    }

    if ( query.numRowsAffected() > 0 )
    {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "We have %d channels with programs in this time period!", query.numRowsAffected());

        m_vectChannelIds.clear();
        while ( query.next() )
            m_vectChannelIds.push_back(query.value(0).toInt());
    }
    else
    {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "There are no programs in this timeframe");
    }
}

void MythTvWrapper::MythTvEpgGrid::readProgramInfo(int channelId, QString programName, QDateTime startTime, QDateTime endTime)
{
    int nColumnSpan = startTime.secsTo(endTime) / m_iQuantInSecs;
    int nColumnStart = m_timeFrom.secsTo(startTime) / m_iQuantInSecs;

    DCE::DataGridCell *pDataGridCell = new DCE::DataGridCell(programName.ascii());
    pDataGridCell->m_Colspan = nColumnSpan;

    SetData(nColumnStart, channelId, pDataGridCell);

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Cell (%s) Span: %d Start at %d. Time: (%s <--> %s)",
                programName.ascii(),
                nColumnSpan, nColumnStart,
                startTime.toString().ascii(),
                endTime.toString().ascii());
}

QString MythTvWrapper::MythTvEpgGrid::getChannelList(int rowStart, int rowCount)
{
    QStringList list;

    while ( rowStart < (int)m_vectChannelIds.size() && rowCount >= 0 )
    {
        list.append(QString::number(m_vectChannelIds[rowStart++]));
        rowCount--;
    }

    return list.join(", ");
}

void MythTvWrapper::MythTvEpgGrid::readDataGridBlock(int rowStart, int rowCount, int colStart, int colCount)
{
    QString strQuery, strTimeFormat;

    QString startDataAsString, endDateAsString;

    strTimeFormat = "yyyy-MM-dd hh:mm:ss";

    QDateTime periodStart = m_timeFrom.addSecs(colStart * m_iQuantInSecs);
    QDateTime periodEnd   = periodStart.addSecs(colCount * m_iQuantInSecs);

    startDataAsString = periodStart.toString(strTimeFormat);
    endDateAsString   = periodEnd.toString(strTimeFormat);

    QString channelList = getChannelList(rowStart, m_bKeepRowHeader ? rowCount - 1 : rowCount);

    if ( channelList.isEmpty() )
    {
        DataGridCell *pDataGridCell = new DataGridCell("No programs are available in the database", "");

        pDataGridCell->m_Colspan = colCount;
        SetData(colStart, 0, pDataGridCell);
		return;
    }

    strQuery.sprintf("SELECT c.channum, p.title, p.starttime, p.endtime"
                     " FROM program p, channel c "
                     " WHERE p.chanid IN (%s) AND "
                           " p.chanid = c.chanid AND "
                           "("
                            "(p.starttime BETWEEN '%s' and '%s') OR "
                            "(p.endtime BETWEEN '%s' and '%s') OR "
                            "(p.starttime < '%s' AND p.endtime > '%s')"
                           ")"
                     " ORDER BY c.channum, p.starttime",
                     getChannelList(rowStart, m_bKeepRowHeader ? rowCount - 1 : rowCount).ascii(),
                     periodStart.toString(strTimeFormat).ascii(),
                     periodEnd.addSecs(-1).toString(strTimeFormat).ascii(),
                     periodStart.addSecs(1).toString(strTimeFormat).ascii(),
                     periodEnd.toString(strTimeFormat).ascii(),
                     periodStart.toString(strTimeFormat).ascii(),
                     periodEnd.toString(strTimeFormat).ascii());

    QSqlQuery query;
    query.exec(strQuery);

//     LoggerWrapper::GetInstance()->Write(LV_STATUS, "Query: %s", strQuery.ascii());
    if ( ! query.isActive() )
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING,
            "The query %s for the channel count failed: %s!",
            strQuery.ascii(),
            query.lastError().text().ascii());

        return;
    }

    this->ClearData();

    if ( m_bKeepRowHeader )
        MakeTimeRow(colStart, colCount);

    if ( m_bKeepColumnHeader )
    {
        MakeChannelColumn( rowStart, rowCount );
        colStart++;
    }

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "RowStart: %d -> RowCount %d", rowStart, rowCount);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "ColStart: %d -> ColCount %d", colStart, colCount);

    if ( query.numRowsAffected() > 0 )
    {
        int currentChannelId = 0, nextChannelId = 0;

		// if they requested a Row Header we will start the actual data from the first row.
		// the initial size is -1 usually because the first channelid will always be different then the prev one (aka the null one)
		// and we will increment it.
        int channelPos = (m_bKeepRowHeader) ? 0 : -1;

        int programColumnSpan = 0;
        int programStartColumn = 0;

        QString programName;
        QDateTime dateStart, dateEnd;

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Date start %s, Date End: %s", periodStart.toString().latin1(), periodEnd.toString().latin1());

        while ( query.next() )
        {
            nextChannelId = query.value(0).toInt();
            programName = query.value(1).toString();
            dateStart = query.value(2).toDateTime();
            dateEnd = query.value(3).toDateTime();

            if ( currentChannelId != nextChannelId )
            {
                channelPos++;
                currentChannelId = nextChannelId;
            }

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "\t\tEntry - start %s, End: %s", dateStart.toString().latin1(), dateEnd.toString().latin1());

			programColumnSpan = dateStart.secsTo(dateEnd) / m_iQuantInSecs;
            programStartColumn = periodStart.secsTo(dateStart) / m_iQuantInSecs;


            if ( programStartColumn < 0 )
            {
                programColumnSpan += programStartColumn;
                programStartColumn = 0;
            }

            DataGridCell *pCell = new DataGridCell(
                    programName.ascii(),
                    QString::number(currentChannelId)
                        .append("|")
                        .append(QString::number(dateStart.date().year()))
                        .append("|")
                        .append(QString::number(dateStart.date().month()))
                        .append("|")
                        .append(QString::number(dateStart.date().day()))
                        .append("|")
                        .append(QString::number(dateStart.time().hour()))
                        .append("|")
                        .append(QString::number(dateStart.time().minute())).ascii());

            pCell->m_Colspan = programColumnSpan;
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Hannibal: %d (+%d), %d, %s, %s", programStartColumn  + colStart, pCell->m_Colspan, channelPos + rowStart, pCell->GetText(), pCell->GetValue());
			SetData(programStartColumn  + colStart, channelPos + rowStart, pCell);

        }
    }
    else
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "There are no programs in this timeframe");
    }
}

void MythTvWrapper::MythTvEpgGrid::MakeChannelColumn(int RowStart, int RowCount)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Trying to make a channel column");
	while ( RowCount != 0 )
    {
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Setting cell (%d, %d), to channel id: %d", 0, RowStart, RowStart );
        SetData(0, RowStart, new DataGridCell(QString::number(RowStart).ascii(), "") );
        RowStart++;
        RowCount--;
    }
}

void MythTvWrapper::MythTvEpgGrid::MakeTimeRow(int ColStart, int ColCount)
{
    QString timeCell;

    QDateTime currentTime = m_timeFrom.addSecs(m_iQuantInSecs * ColStart);

    while ( ColCount != 0 && ColStart + ColCount < m_iColumns)
    {
        DataGridCell *pCell = new DataGridCell(currentTime.toString("MM/dd hh:mm"), "");
        pCell->m_Colspan = 4;
        SetData(ColStart, 0, pCell);
        currentTime = currentTime.addSecs(m_iQuantInSecs * 4);
        ColStart+=4;
        ColCount-=4;

        if ( ColCount < 0 )
            ColCount = 0;
    }
    return;
}

void MythTvWrapper::MythTvEpgGrid::ToData(string GridID,int &Size, char* &Data, int *ColStart, int *RowStart, int ColCount, int RowCount)
{
    readDataGridBlock(*RowStart, RowCount, *ColStart, ColCount);

    /** This is required because the ToData doesn't honor the Get{Cols,Rows} calls. */
    m_TotalColumns = GetCols();
    m_TotalRows = GetRows();

    DataGridTable::ToData(GridID, Size, Data, ColStart, RowStart, ColCount, RowCount);
}

void MythTvWrapper::MythTvEpgGrid::setMythWrapper(MythTvWrapper *pWrapper)
{
    m_pMythWrapper = pWrapper;
}

DataGridTable *MythTvWrapper::createShowsDataGrid(string GridID, QDateTime startTime, QDateTime endTime)
{
    MythTvEpgGrid *pNewGrid = new MythTvEpgGrid();

    pNewGrid->setMythWrapper(this);
    pNewGrid->setGridBoundaries(startTime, endTime);

    return pNewGrid;
}

DataGridTable *MythTvWrapper::createShowsForMobiles(string GridID, QDateTime currentTime)
{
    QString strTimeFormat = "yyyy-MM-dd hh:mm:ss";
    QString strQuery;

    strQuery.sprintf(
                     "SELECT c.chanid, c.name, p.title, p.starttime, p.endtime "
                     " FROM program p "
                        " INNER JOIN channel c ON c.chanid = p.chanid "
                        " WHERE '%s' BETWEEN p.starttime and p.endtime "
                        " ORDER BY p.chanid",
                        currentTime.toString(strTimeFormat).ascii());

    QSqlQuery query;
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Running query: %s", strQuery.latin1());

    query.exec(strQuery);

    if ( ! query.isActive() )
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING,
            "The query %s for the channel count failed: %s!",
            strQuery.ascii(),
            query.lastError().text().ascii());

        return NULL;
    }

    if ( query.numRowsAffected() > 0 )
    {
        QString programName;
        QString channelName;
        QDateTime dateStart, dateEnd;

        int channelId;

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "We have %d rows!", query.numRowsAffected());
        DataGridTable *pDataGridTable = new DataGridTable();
        while ( query.next() )
        {
            channelId = query.value(0).toInt();
            channelName = query.value(1).toString();
            programName = query.value(2).toString();
            dateStart = query.value(3).toDateTime();
            dateEnd = query.value(4).toDateTime();

            DataGridCell *pCell = new DataGridCell(
                    QString("%1 - %2:%3 %4:%5\n%6")
                        .arg(channelName)
                        .arg(dateStart.time().hour())
                        .arg(dateStart.time().minute())
                        .arg(dateEnd.time().hour())
                        .arg(dateEnd.time().minute())
                        .arg(programName)
                        .ascii(),
                    QString("%1|%2|%3|%4|%5|%6")
                        .arg(channelId)
                        .arg(dateStart.date().year())
                        .arg(dateStart.date().month())
                        .arg(dateStart.date().day())
                        .arg(dateStart.time().hour())
                        .arg(dateStart.time().minute())
                        .ascii());

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Setting data in grid [%d, %d] -> %s!", 0, query.at(), pCell->GetText());
            pDataGridTable->SetData(0, query.at(), pCell);
        }

        return pDataGridTable;
    }

    DataGridTable *pDataGridTable = new DataGridTable();

    pDataGridTable->SetData(0, 0, new DataGridCell("There are no shows\nin the database at\nthis time.", ""));

    return pDataGridTable;
}

bool MythTvWrapper::decodeProgramStartTime(string sValue, int &nChanId, int &tmYear, int &tmMonth, int &tmDay, int &tmHour, int &tmMinute)
{
    vector<string> numbers;
    StringUtils::Tokenize(sValue, "|", numbers);

    if ( numbers.size() != 6 )
    {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "We can't decode this: %s", sValue.c_str());
        return false;
    }

    nChanId  = atoi(numbers[0].c_str());
    tmYear   = atoi(numbers[1].c_str());
    tmMonth  = atoi(numbers[2].c_str());
    tmDay    = atoi(numbers[3].c_str());
    tmHour   = atoi(numbers[4].c_str());
    tmMinute = atoi(numbers[5].c_str());

    return true;
}
WatchTVRequestResult MythTvWrapper::ProcessWatchTvRequest(string showStartTimeEncoded)
{
    int iChanId;
    int tmYear, tmMonth, tmDay, tmHour, tmMinute;

    if ( ! decodeProgramStartTime(showStartTimeEncoded,
                                  iChanId,
                                  tmYear, tmMonth, tmDay,
                                  tmHour, tmMinute))
        if ( showStartTimeEncoded == "+1" )
        {
            DCE::CMD_PIP_Channel_Up channelUp(
                    m_pDCEDeviceWrapper->m_dwPK_Device,
                    ((MythTV_PlugIn*)m_pDCEDeviceWrapper)->m_dwTargetDevice);

            m_pDCEDeviceWrapper->SendCommand(channelUp);
            return WatchTVResult_Tuned;
        }
        else if (showStartTimeEncoded == "-1" )
        {
            DCE::CMD_PIP_Channel_Down channelDown(
                    m_pDCEDeviceWrapper->m_dwPK_Device,
                    ((MythTV_PlugIn*)m_pDCEDeviceWrapper)->m_dwTargetDevice);

            m_pDCEDeviceWrapper->SendCommand(channelDown);
            return WatchTVResult_Tuned;
        }
        else
            return WatchTVResult_Ignored;


    WatchTVRequestResult result;
    if ( (result = ProcessWatchTvRequest(iChanId, tmYear, tmMonth, tmDay, tmHour, tmMinute)) == WatchTVResult_Tuned )
    {
        DCE::CMD_Tune_to_channel tuneToChannel(
                m_pDCEDeviceWrapper->m_dwPK_Device,
                ((MythTV_PlugIn*)m_pDCEDeviceWrapper)->m_dwTargetDevice,
				"",
                showStartTimeEncoded);

        m_pDCEDeviceWrapper->SendCommand(tuneToChannel);
    }

    return result;
}

WatchTVRequestResult MythTvWrapper::ProcessWatchTvRequest(int iChanId, int nYear, int nMonth, int nDay, int nHour, int nMinute)
{
    QDateTime programStartTime = QDateTime(QDate(nYear, nMonth, nDay), QTime(nHour, nMinute));

//     ProgramInfo *programInfo = ProgramInfo::GetProgramAtDateTime(
//                                                 QSqlDatabase::database(),
//                                                 QString::number(iChanId),
//                                                 programStartTime);

    if ( QDateTime::currentDateTime() > programStartTime )
        return WatchTVResult_Tuned;
    else
        return WatchTVResult_InTheFuture;
}

ScheduleRecordTvResult MythTvWrapper::ProcessAddRecordingRequest(string showStartTimeEncoded)
{
    int iChanId;
    int tmYear, tmMonth, tmDay, tmHour, tmMinute;

    if ( ! decodeProgramStartTime(showStartTimeEncoded,
                                  iChanId,
                                  tmYear, tmMonth, tmDay,
                                  tmHour, tmMinute))
        return ScheduleRecordTVResult_Failed;

    return ProcessAddRecordingRequest(iChanId, tmYear, tmMonth, tmDay, tmHour, tmMinute);
}

ScheduleRecordTvResult MythTvWrapper::ProcessAddRecordingRequest(int channelId, int year, int month, int day, int hour, int minute)
{
    QDateTime programStartTime = QDateTime(QDate(year, month, day), QTime(hour, minute));

    ProgramInfo *programInfo;

    programInfo = ProgramInfo::GetProgramAtDateTime(
                                        QString::number(channelId),
                                        programStartTime);

	if ( programInfo == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "I could not find the data for the show that i want to record. Is the MythTV Backed and/or proxy working ?");
		return ScheduleRecordTVResult_Failed;
	}
    programInfo->ApplyRecordStateChange(kSingleRecord);

    vector<ProgramInfo *> *conflictsWith = RemoteGetConflictList(programInfo);

    if ( conflictsWith == NULL )
    {
//         m_pDCEDeviceWrapper->EVENT_Error_Occured("The connection to master server was broken!");
        return ScheduleRecordTVResult_Failed;
    }

    if ( conflictsWith->size() != 0 )
    {
        vector<ProgramInfo*>::iterator itPrograms;

        for ( itPrograms = conflictsWith->begin(); itPrograms != conflictsWith->end(); itPrograms++ )
            delete *itPrograms;

        delete conflictsWith;
        return ScheduleRecordTVResult_WithConflicts;
    }

    delete conflictsWith;
    return ScheduleRecordTVResult_Success;
}

bool MythTvWrapper::GetCurrentChannelProgram(int channelID, string &channelName, string &programName, string &programComment)
{
	QDateTime programStartTime = QDateTime::currentDateTime();

    QString strQuery = QString(
		"SELECT program.chanid, program.starttime, program.endtime, "
			"program.title, program.subtitle, program.description, "
			"program.category, channel.channum, channel.callsign, "
			"channel.name, program.previouslyshown, channel.commfree, "
			"channel.outputfilters, program.seriesid, program.programid, "
			"program.stars, program.originalairdate "
		"FROM program "
			"LEFT JOIN channel ON program.chanid = channel.chanid "
		"WHERE channel.channum = %1 AND "
			"starttime < %2 AND %3 < endtime "
		"GROUP BY program.starttime, channel.channum, channel.callsign "
		"ORDER BY program.starttime, channum + 0 LIMIT 1")
			.arg(channelID).arg(programStartTime.toString("yyyyMMddhhmm50")).arg(programStartTime.toString("yyyyMMddhhmm50"));

    QSqlQuery query;
    query.exec(strQuery);

	if ( ! query.isActive() )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Program info for current show on channel %d was null (for date: %s)!", channelID, programStartTime.toString(Qt::ISODate).latin1());
		return false;
	}

	if ( query.numRowsAffected() > 0 && query.next() )
	{
		channelName = query.value(9).toString().latin1();
		programName = query.value(3).toString().latin1();
		programComment = query.value(5).toString().latin1();
		return true;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "There are no programs in this timeframe");
	return false;
}

#endif
