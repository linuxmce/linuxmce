/**
 *
 * @file MythTvWrapper.h
 * @brief header file for the MythTvWrapper class
 *
 */

#ifndef MYTHTVEPGWRAPPER_H
#define MYTHTVEPGWRAPPER_H

#include "DataGrid.h"

#include <vector>

#include <qdatetime.h>
#include <qstring.h>

#include "Message.h"
#include "Command_Impl.h"

class QApplication;

using namespace std;
using namespace DCE;

/**
 * @brief the result of the 'watch tv' request
 */

typedef enum {
    WatchTVResult_Failed,
    WatchTVResult_Ignored,
    WatchTVResult_Tuned,
    WatchTVResult_InTheFuture
} WatchTVRequestResult;

/**
 * @brief the result of the 'schedule record tv'
 * @todo ask
 */

typedef enum {
    ScheduleRecordTVResult_Success,
    ScheduleRecordTVResult_Failed,
    ScheduleRecordTVResult_WithConflicts,
} ScheduleRecordTvResult;

/**
 * @brief documentation
 * @todo complete documentation
 */

#ifndef MythTV_PlugIn_h
class MythTV_PlugIn;
#endif

class MythTvWrapper
{
    QApplication  * m_pQApplication;
    Command_Impl  * m_pDCEDeviceWrapper;

protected:

    bool initMythTVGlobalContext();

    class MythTvEpgGrid: public DataGridTable
    {
        MythTvWrapper *m_pMythWrapper;

        QDateTime m_timeFrom, m_timeTo;

        vector<int> m_vectChannelIds;
        int m_iQuantInSecs;
        int m_iColumns;

    private:
        void setMythWrapper(MythTvWrapper *pWrapper);

    protected:
        QString getChannelList(int rowStart, int rowCount);
        void readChannelData();
        void readProgramInfo(int channelId, QString programName, QDateTime startTime, QDateTime endTime);
        void readDataGridBlock(int rowStart, int rowCount, int colStart, int colCount);
        void MakeTimeRow(int ColStart, int ColCount);
        void MakeChannelColumn(int RowStart, int RowCount);

    public:

    /** @brief constructor */
	    MythTvEpgGrid();

        virtual void setGridBoundaries(QDateTime startTime, QDateTime endTime);
        virtual int GetRows();
        virtual int GetCols();

        virtual void ToData(string GridID,int &Size, char* &Data, int *ColStart, int *RowStart, int ColCount, int RowCount);

    friend class MythTvWrapper;
    };

    bool decodeProgramStartTime(string sValue, int &nChanId, int &tmYear, int &tmMonth, int &tmDay, int &tmHour, int &tmMinute);
public:

    /** @brief constructor */
    MythTvWrapper(Command_Impl *pCommandImpl);
	~MythTvWrapper();

    /** Methods used by the MythTV_PlugIn */

    DataGridTable *createShowsDataGrid(string GridID, QDateTime startTime, QDateTime endTime);
    DataGridTable *createShowsForMobiles(string GridID, QDateTime currentTime);

    WatchTVRequestResult ProcessWatchTvRequest(string showStartTimeEncoded);
    WatchTVRequestResult ProcessWatchTvRequest(int channelId, int year, int month, int day, int hour, int minute);

    ScheduleRecordTvResult ProcessAddRecordingRequest(string showStartTimeEncoded);
    ScheduleRecordTvResult ProcessAddRecordingRequest(int channelId, int year, int month, int day, int hour, int minute);

	bool GetCurrentChannelProgram(int channelID, string &channelName, string &programName, string &programComment);

    friend class MythTV_PlugIn;
};

/** @test
// class MythTvWrapper: public DataGridTable
// {
//     QApplication *m_pQApplication;
//     QDateTime m_timeStart, m_timeEnd;
//
//     vector<int> m_vectChannelIds;
//     int m_iQuantInSecs;
//     int m_iColumns;
//
// //     map<int, map<int, pair<string, string> > > m_gridBlock;
//
// protected:
//     QString getChannelList(int rowStart, int rowCount);
//     void readChannelData();
//     void cleanChannelStorage(int channelId);
//     void readProgramInfo(int channelId, QString programName, QDateTime startTime, QDateTime endTime);
//     void readDataGridBlock(int rowStart, int rowCount, int colStart, int colCount);
//
//
// public:
//     MythTvEPGWrapper();
//
//     virtual void setGridBoundaries(QDateTime startTime, QDateTime endTime);
//     virtual int GetRows();  // The total number of rows in the whole grid
//     virtual int GetCols();  // The total number of columns in the whole grid
//
//
//     virtual void ToData(string GridID,int &Size, char* &Data, int ColStart, int RowStart, int ColCount, int RowCount);
// };
*/
#endif

