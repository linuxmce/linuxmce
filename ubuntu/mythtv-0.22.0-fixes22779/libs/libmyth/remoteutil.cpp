#include <unistd.h>

#include <QFileInfo>
#include <QFile>
#include <QDir>

#include "remoteutil.h"
#include "programinfo.h"
#include "mythcontext.h"
#include "decodeencode.h"
#include "storagegroup.h"

vector<ProgramInfo *> *RemoteGetRecordedList(bool deltype)
{
    QString str = "QUERY_RECORDINGS ";
    if (deltype)
        str += "Delete";
    else
        str += "Play";

    QStringList strlist(str);

    vector<ProgramInfo *> *info = new vector<ProgramInfo *>;

    if (!RemoteGetRecordingList(info, strlist))
    {
        delete info;
        return NULL;
    }
 
    return info;
}

/** \fn RemoteGetFreeSpace(void)
 *  \brief Returns total and used space in kilobytes for each backend.
 */
vector<FileSystemInfo> RemoteGetFreeSpace(void)
{
    FileSystemInfo fsInfo;
    vector<FileSystemInfo> fsInfos;
    QStringList strlist(QString("QUERY_FREE_SPACE_LIST"));

    if (gContext->SendReceiveStringList(strlist))
    {
        QStringList::const_iterator it = strlist.begin();
        while (it != strlist.end())
        {
            fsInfo.hostname = *(it++);
            fsInfo.directory = *(it++);
            fsInfo.isLocal = (*(it++)).toInt();
            fsInfo.fsID = (*(it++)).toInt();
            fsInfo.dirID = (*(it++)).toInt();
            fsInfo.blocksize = (*(it++)).toInt();
            fsInfo.totalSpaceKB = decodeLongLong(strlist, it);
            fsInfo.usedSpaceKB = decodeLongLong(strlist, it);
            fsInfos.push_back(fsInfo);
        }
    }

    return fsInfos;
}

bool RemoteGetLoad(float load[3])
{
    QStringList strlist(QString("QUERY_LOAD"));

    if (gContext->SendReceiveStringList(strlist))
    {
        load[0] = strlist[0].toFloat();
        load[1] = strlist[1].toFloat();
        load[2] = strlist[2].toFloat();
        return true;
    }

    return false;
}

bool RemoteGetUptime(time_t &uptime)
{
    QStringList strlist(QString("QUERY_UPTIME"));

    if (!gContext->SendReceiveStringList(strlist))
        return false;

    if (!strlist[0].at(0).isNumber())
        return false;

    if (sizeof(time_t) == sizeof(int))
        uptime = strlist[0].toUInt();
    else if (sizeof(time_t) == sizeof(long))
        uptime = strlist[0].toULong();
    else if (sizeof(time_t) == sizeof(long long))
        uptime = strlist[0].toULongLong();

    return true;
}

bool RemoteGetMemStats(int &totalMB, int &freeMB, int &totalVM, int &freeVM)
{
    QStringList strlist(QString("QUERY_MEMSTATS"));

    if (gContext->SendReceiveStringList(strlist))
    {
        totalMB = strlist[0].toInt();
        freeMB  = strlist[1].toInt();
        totalVM = strlist[2].toInt();
        freeVM  = strlist[3].toInt();
        return true;
    }

    return false;
}

bool RemoteCheckFile(ProgramInfo *pginfo, bool checkSlaves)
{
    QStringList strlist("QUERY_CHECKFILE");
    strlist << QString::number((int)checkSlaves);
    pginfo->ToStringList(strlist);

    if ((!gContext->SendReceiveStringList(strlist)) ||
        (!strlist[0].toInt()))
        return false;

    // Only modify the pathname if the recording file is available locally on
    // this host
    QString localpath = strlist[1];
    QFile checkFile(localpath);
    if (checkFile.exists())
        pginfo->pathname = localpath;

    return true;
}

bool RemoteDeleteRecording(
    const ProgramInfo *pginfo, bool forgetHistory, bool forceMetadataDelete)
{
    bool result = true;
    QStringList strlist(
        forceMetadataDelete ? "FORCE_DELETE_RECORDING" : "DELETE_RECORDING");

    pginfo->ToStringList(strlist);

    if (!gContext->SendReceiveStringList(strlist) || strlist.empty())
        result = false;
    else if (strlist[0].toInt() == -2)
        result = false;

    if (!result)
    {
        VERBOSE(VB_IMPORTANT, QString("Failed to delete recording %1:%2")
                .arg(pginfo->title).arg(pginfo->subtitle));
    }

    // We don't care if the recording is successfully deleted..
    if (forgetHistory)
    {
        strlist = QStringList("FORGET_RECORDING");
        pginfo->ToStringList(strlist);

        if (!gContext->SendReceiveStringList(strlist))
        {
            VERBOSE(VB_IMPORTANT, QString("Failed to forget recording %1:%2")
                    .arg(pginfo->title).arg(pginfo->subtitle));
        }
    }

    return result;
}

bool RemoteUndeleteRecording(const ProgramInfo *pginfo)
{
    bool result = false;

    bool undelete_possible = 
            gContext->GetNumSetting("AutoExpireInsteadOfDelete", 0);

    if (!undelete_possible)
        return result;

    QStringList strlist(QString("UNDELETE_RECORDING"));
    pginfo->ToStringList(strlist);

    gContext->SendReceiveStringList(strlist);

    if (strlist[0].toInt() == 0)
        result = true;

    return result;
}

void RemoteGetAllScheduledRecordings(vector<ProgramInfo *> &scheduledlist)
{
    QStringList strList(QString("QUERY_GETALLSCHEDULED"));
    RemoteGetRecordingList(&scheduledlist, strList);
}

void RemoteGetAllExpiringRecordings(vector<ProgramInfo *> &expiringlist)
{
    QStringList strList(QString("QUERY_GETEXPIRING"));
    RemoteGetRecordingList(&expiringlist, strList);
}

int RemoteGetRecordingList(vector<ProgramInfo *> *reclist, QStringList &strList)
{
    if (!gContext->SendReceiveStringList(strList))
        return 0;

    int numrecordings = strList[0].toInt();

    if (numrecordings > 0)
    {
        if (numrecordings * NUMPROGRAMLINES + 1 > (int)strList.size())
        {
            cerr << "length mismatch between programinfo\n";
            return 0;
        }

        QStringList::const_iterator it = strList.begin() + 1;
        for (int i = 0; i < numrecordings; i++)
        {
            ProgramInfo *pginfo = new ProgramInfo();
            pginfo->FromStringList(it, strList.end());
            reclist->push_back(pginfo);
        }
    }

    return numrecordings;
}

vector<ProgramInfo *> *RemoteGetConflictList(const ProgramInfo *pginfo)
{
    QString cmd = QString("QUERY_GETCONFLICTING");
    QStringList strlist( cmd );
    pginfo->ToStringList(strlist);

    vector<ProgramInfo *> *retlist = new vector<ProgramInfo *>;

    RemoteGetRecordingList(retlist, strlist);
    return retlist;
}

vector<uint> RemoteRequestFreeRecorderList(void)
{
    vector<uint> list;

    QStringList strlist("GET_FREE_RECORDER_LIST");

    if (!gContext->SendReceiveStringList(strlist, true))
        return list;

    QStringList::const_iterator it = strlist.begin();
    for (; it != strlist.end(); ++it) 
        list.push_back((*it).toUInt());

    return list;
}

void RemoteSendMessage(const QString &message)
{
    QStringList strlist( "MESSAGE" );
    strlist << message;

    gContext->SendReceiveStringList(strlist);
}

void RemoteGeneratePreviewPixmap(const ProgramInfo *pginfo)
{
    QStringList strlist( "QUERY_GENPIXMAP" );
    pginfo->ToStringList(strlist);

    gContext->SendReceiveStringList(strlist);
}
    
QDateTime RemoteGetPreviewLastModified(const ProgramInfo *pginfo)
{
    QDateTime retdatetime;

    QStringList strlist( "QUERY_PIXMAP_LASTMODIFIED" );
    pginfo->ToStringList(strlist);
    
    if (!gContext->SendReceiveStringList(strlist))
        return retdatetime;

    if (!strlist.empty() && strlist[0] != "BAD")
    {
        uint timet = strlist[0].toUInt();
        retdatetime.setTime_t(timet);
    }
        
    return retdatetime;
}

/// Download preview & get timestamp if newer than cachefile's
/// last modified time, otherwise just get the timestamp
QDateTime RemoteGetPreviewIfModified(
    const ProgramInfo &pginfo, const QString &cachefile)
{
    QString loc_err("RemoteGetPreviewIfModified, Error: ");

    QDateTime cacheLastModified;
    QFileInfo cachefileinfo(cachefile);
    if (cachefileinfo.exists())
        cacheLastModified = cachefileinfo.lastModified();

    QStringList strlist("QUERY_PIXMAP_GET_IF_MODIFIED");
    strlist << ((cacheLastModified.isValid()) ? // unix secs, UTC
                QString::number(cacheLastModified.toTime_t()) : QString("-1"));
    strlist << QString::number(200 * 1024); // max size of preview file
    pginfo.ToStringList(strlist);

    if (!gContext->SendReceiveStringList(strlist) ||
        strlist.empty() || strlist[0] == "ERROR")
    {
        VERBOSE(VB_IMPORTANT, loc_err +
                QString("Remote error") +
                ((strlist.size() >= 2) ?
                 (QString(":\n\t\t\t") + strlist[1]) : QString("")));

        return QDateTime();
    }

    if (strlist[0] == "WARNING")
    {
        VERBOSE(VB_NETWORK, QString("RemoteGetPreviewIfModified, Warning: ") +
                QString("Remote warning") +
                ((strlist.size() >= 2) ?
                 (QString(":\n\t\t\t") + strlist[1]) : QString("")));

        return QDateTime();
    }

    QDateTime retdatetime;
    qlonglong timet = strlist[0].toLongLong();
    if (timet >= 0)
        retdatetime.setTime_t(timet);

    if (strlist.size() < 4)
    {
        return retdatetime;
    }

    size_t  length     = strlist[1].toLongLong();
    quint16 checksum16 = strlist[2].toUInt();
    QByteArray data = QByteArray::fromBase64(strlist[3].toAscii());
    if ((size_t) data.size() < length)
    { // (note data.size() may be up to 3 bytes longer after decoding
        VERBOSE(VB_IMPORTANT, loc_err +
                QString("Preview size check failed %1 < %2")
                .arg(data.size()).arg(length));
        return QDateTime();
    }

    if (checksum16 != qChecksum(data.constData(), data.size()))
    {
        VERBOSE(VB_IMPORTANT, loc_err + "Preview checksum failed");
        return QDateTime();
    }

    QString pdir(cachefile.section("/", 0, -2));
    QDir cfd(pdir);
    if (!cfd.exists() && !cfd.mkdir(pdir))
    {
        VERBOSE(VB_IMPORTANT, loc_err +
                QString("Unable to create remote cache directory '%1'")
                .arg(pdir));

        return QDateTime();
    }

    QFile file(cachefile);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Truncate))
    {
        VERBOSE(VB_IMPORTANT, loc_err +
                QString("Unable to open cached "
                        "preview file for writing '%1'")
                .arg(cachefile));

        return QDateTime();
    }

    off_t offset = 0;
    size_t remaining = length;
    uint failure_cnt = 0;
    while ((remaining > 0) && (failure_cnt < 5))
    {
        ssize_t written = file.write(data.data() + offset, remaining);
        if (written < 0)
        {
            failure_cnt++;
            usleep(50000);
            continue;
        }

        failure_cnt  = 0;
        offset      += written;
        remaining   -= written;
    }

    if (remaining)
    {
        VERBOSE(VB_IMPORTANT, loc_err +
                QString("Failed to write cached preview file '%1'")
                .arg(cachefile));

        file.resize(0); // in case unlink fails..
        file.remove();  // closes fd
        return QDateTime();
    }

    file.close();

    return retdatetime;
}

void RemoteFillProginfo(ProgramInfo *pginfo, const QString &playbackhostname)
{
    QStringList strlist( "FILL_PROGRAM_INFO" );
    strlist << playbackhostname;
    pginfo->ToStringList(strlist);

    if (gContext->SendReceiveStringList(strlist))
        pginfo->FromStringList(strlist, 0);
}

QStringList RemoteRecordings(void)
{
    QStringList strlist("QUERY_ISRECORDING");

    if (!gContext->SendReceiveStringList(strlist, false, false))
    {
        QStringList empty;
        empty << "0" << "0";
        return empty;
    }

    return strlist;
}

int RemoteGetRecordingMask(void)
{
    int mask = 0;

    QString cmd = "QUERY_ISRECORDING";

    QStringList strlist( cmd );

    if (!gContext->SendReceiveStringList(strlist))
        return mask;

    if (strlist.empty())
        return 0;

    int recCount = strlist[0].toInt();

    for (int i = 0, j = 0; j < recCount; i++)
    {
        cmd = QString("QUERY_RECORDER %1").arg(i + 1);

        strlist = QStringList( cmd );
        strlist << "IS_RECORDING";

        if (gContext->SendReceiveStringList(strlist) && !strlist.empty())
        {
            if (strlist[0].toInt())
            {
                mask |= 1<<i;
                j++;       // count active recorder
            }
        }
        else
        {
            break;
        }
    }

    return mask;
}

int RemoteGetFreeRecorderCount(void)
{
    QStringList strlist( "GET_FREE_RECORDER_COUNT" );

    if (!gContext->SendReceiveStringList(strlist, true))
        return 0;

    if (strlist.empty())
        return 0;

    if (strlist[0] == "UNKNOWN_COMMAND")
    {
        cerr << "Unknown command GET_FREE_RECORDER_COUNT, upgrade "
                "your backend version." << endl;
        return 0;
    }

    return strlist[0].toInt();
}

static QMutex sgroupMapLock;
static QHash <QString, QString>sgroupMap;

void RemoteClearSGMap(void)
{
    QMutexLocker locker(&sgroupMapLock);
    sgroupMap.clear();
}

QString GetHostSGToUse(QString host, QString sgroup)
{
    QString tmpGroup = sgroup;
    QString groupKey = QString("%1:%2").arg(sgroup, host);

    QMutexLocker locker(&sgroupMapLock);

    if (sgroupMap.contains(groupKey))
    {
        tmpGroup = sgroupMap[groupKey];
    }
    else
    {
        if (StorageGroup::FindDirs(sgroup, host))
        {
            sgroupMap[groupKey] = sgroup;
        }
        else
        {
            VERBOSE(VB_FILE+VB_EXTRA, QString("GetHostSGToUse(): "
                    "falling back to Videos Storage Group for host %1 "
                    "since it does not have a %2 Storage Group.")
                    .arg(host).arg(sgroup));

            tmpGroup = "Videos";
            sgroupMap[groupKey] = tmpGroup;
        }
    }

    return tmpGroup;
}

bool RemoteGetFileList(QString host, QString path, QStringList* list,
                       QString sgroup, bool fileNamesOnly)
{

    // Make sure the list is empty when we get started
    list->clear();

    if (sgroup.isEmpty())
        sgroup = "Videos";

    *list << "QUERY_SG_GETFILELIST";
    *list << host;
    *list << GetHostSGToUse(host, sgroup);
    *list << path;
    *list << QString::number(fileNamesOnly);

    bool ok = gContext->SendReceiveStringList(*list);

// Should the SLAVE UNREACH test be here ?
    return ok;
}

QString RemoteGenFileURL(QString sgroup, QString host, QString path)
{
    return QString("myth://%1@").arg(GetHostSGToUse(host, sgroup)) +
              gContext->GetSettingOnHost("BackendServerIP", host) + ":" +
              gContext->GetSettingOnHost("BackendServerPort", host) + "/" +
              path;

}

/**
 * Get recorder for a programme.
 *
 * \return recordernum if pginfo recording in progress, else 0
 */
int RemoteCheckForRecording(const ProgramInfo *pginfo)
{
    QStringList strlist( QString("CHECK_RECORDING") );
    pginfo->ToStringList(strlist);

    if (gContext->SendReceiveStringList(strlist) && !strlist.empty())
        return strlist[0].toInt();

    return 0;
}

/**
 * Get status of an individual programme (with pre-post roll?).
 *
 * \retval  0  Not Recording
 * \retval  1  Recording
 * \retval  2  Under-Record
 * \retval  3  Over-Record
 */
int RemoteGetRecordingStatus(
    const ProgramInfo *pginfo, int overrecsecs, int underrecsecs)
{
    QDateTime curtime = QDateTime::currentDateTime();

    int retval = 0;

    if (pginfo)
    {
        if (curtime >= pginfo->startts.addSecs(-underrecsecs) &&
            curtime < pginfo->endts.addSecs(overrecsecs))
        {
            if (curtime >= pginfo->startts && curtime < pginfo->endts)
                retval = 1;
            else if (curtime < pginfo->startts && 
                     RemoteCheckForRecording(pginfo) > 0)
                retval = 2;
            else if (curtime > pginfo->endts && 
                     RemoteCheckForRecording(pginfo) > 0)
                retval = 3;
        }
    }

    return retval;
}

/**
 * \brief return list of currently recording shows
 */
vector<ProgramInfo *> *RemoteGetCurrentlyRecordingList(void)
{
    QString str = "QUERY_RECORDINGS ";
    str += "Recording";
    QStringList strlist( str );

    vector<ProgramInfo *> *reclist = new vector<ProgramInfo *>;
    vector<ProgramInfo *> *info = new vector<ProgramInfo *>;
    if (!RemoteGetRecordingList(info, strlist))
    {
        if (info)
            delete info;
        return reclist;
    }

    ProgramInfo *p = NULL;
    vector<ProgramInfo *>::iterator it = info->begin();
    // make sure whatever remotegetrecordinglist returned
    // only has rsRecording shows
    for ( ; it != info->end(); it++)
    {
        p = *it;
        if (p->recstatus == rsRecording || (p->recstatus == rsRecorded
                                            && p->recgroup == "LiveTV"))
            reclist->push_back(new ProgramInfo(*p));
    }
    
    while (!info->empty())
    {
        delete info->back();
        info->pop_back();
    }
    if (info)
        delete info;

    return reclist; 
}

/* vim: set expandtab tabstop=4 shiftwidth=4: */
