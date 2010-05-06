#include <list>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <cerrno>
#include <memory>
using namespace std;

#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include "mythconfig.h"

#ifndef USING_MINGW
#include <sys/ioctl.h>
#endif

#include <sys/stat.h>
#ifdef __linux__
#  include <sys/vfs.h>
#else // if !__linux__
#  include <sys/param.h>
#  ifndef USING_MINGW
#    include <sys/mount.h>
#  endif // USING_MINGW
#endif // !__linux__

#include <QCoreApplication>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QThread>
#include <QWaitCondition>
#include <QRegExp>
#include <QEvent>
#include <QUrl>
#include <QTcpServer>
#include <QTimer>

#include "exitcodes.h"
#include "mythcontext.h"
#include "mythverbose.h"
#include "mythversion.h"
#include "decodeencode.h"
#include "mythdb.h"
#include "mainserver.h"
#include "server.h"
#include "scheduler.h"
#include "backendutil.h"
#include "programinfo.h"
#include "programlist.h"
#include "recordinginfo.h"
#include "recordinglist.h"
#include "recordingrule.h"
#include "scheduledrecording.h"
#include "jobqueue.h"
#include "autoexpire.h"
#include "previewgenerator.h"
#include "storagegroup.h"
#include "compat.h"
#include "RingBuffer.h"
#include "remotefile.h"
#include "mythsystemevent.h"

/** Milliseconds to wait for an existing thread from
 *  process request thread pool.
 */
#define PRT_TIMEOUT 10
/** Number of threads in process request thread pool at startup. */
#define PRT_STARTUP_THREAD_COUNT 5

#define LOC      QString("MainServer: ")
#define LOC_WARN QString("MainServer, Warning: ")
#define LOC_ERR  QString("MainServer, Error: ")

namespace {

int delete_file_immediately(const QString &filename,
                            bool followLinks, bool checkexists)
{
    /* Return 0 for success, non-zero for error. */
    QFile checkFile(filename);
    int success1, success2;

    VERBOSE(VB_FILE, QString("About to delete file: %1").arg(filename));
    success1 = true;
    success2 = true;
    if (followLinks)
    {
        QFileInfo finfo(filename);
        if (finfo.isSymLink())
        {
            QString linktext = getSymlinkTarget(filename);

            QFile target(linktext);
            if (!(success1 = target.remove()))
            {
                VERBOSE(VB_IMPORTANT,
                        QString("Error deleting '%1' -> '%2'")
                        .arg(filename).arg(linktext) + ENO);
            }
        }
    }
    if ((!checkexists || checkFile.exists()) &&
            !(success2 = checkFile.remove()))
    {
        VERBOSE(VB_IMPORTANT, QString("Error deleting '%1': %2")
                .arg(filename).arg(strerror(errno)));
    }
    return success1 && success2 ? 0 : -1;
}

};

QMutex MainServer::truncate_and_close_lock;
const uint MainServer::kMasterServerReconnectTimeout = 1000; //ms

class ProcessRequestThread : public QThread
{
  public:
    ProcessRequestThread(MainServer *ms) :
        parent(ms), socket(NULL), threadlives(false) {}

    void setup(MythSocket *sock)
    {
        QMutexLocker locker(&lock);
        socket = sock;
        socket->UpRef();
        waitCond.wakeAll();
    }

    void killit(void)
    {
        QMutexLocker locker(&lock);
        threadlives = false;
        waitCond.wakeAll();
    }

    virtual void run(void)
    {
        QMutexLocker locker(&lock);
        threadlives = true;
        waitCond.wakeAll(); // Signal to creating thread

        while (true)
        {
            waitCond.wait(locker.mutex());

            if (!threadlives)
                break;

            if (!socket)
                continue;

            parent->ProcessRequest(socket);
            socket->DownRef();
            socket = NULL;
            parent->MarkUnused(this);
        }
    }

    QMutex lock;
    QWaitCondition waitCond;

  private:
    MainServer *parent;

    MythSocket *socket;

    bool threadlives;
};

MainServer::MainServer(bool master, int port,
                       QMap<int, EncoderLink *> *tvList,
                       Scheduler *sched, AutoExpire *expirer) :
    encoderList(tvList), mythserver(NULL), masterServerReconnect(NULL),
    masterServer(NULL), ismaster(master), masterBackendOverride(false),
    m_sched(sched), m_expirer(expirer), deferredDeleteTimer(NULL),
    autoexpireUpdateTimer(NULL), m_exitCode(BACKEND_EXIT_OK)
{
    AutoExpire::Update(true);

    for (int i = 0; i < PRT_STARTUP_THREAD_COUNT; i++)
    {
        ProcessRequestThread *prt = new ProcessRequestThread(this);
        prt->lock.lock();
        prt->start();
        prt->waitCond.wait(&prt->lock);
        prt->lock.unlock();
        threadPool.push_back(prt);
    }

    masterBackendOverride = gContext->GetNumSetting("MasterBackendOverride", 0);

    mythserver = new MythServer();
    if (!mythserver->listen(QHostAddress::Any, port))
    {
        VERBOSE(VB_IMPORTANT, QString("Failed to bind port %1. Exiting.")
                .arg(port));
        SetExitCode(BACKEND_BUGGY_EXIT_NO_BIND_MAIN, false);
        return;
    }

    connect(mythserver, SIGNAL(newConnect(MythSocket *)),
            SLOT(newConnection(MythSocket *)));

    gContext->addListener(this);

    if (!ismaster)
    {
        masterServerReconnect = new QTimer(this);
        masterServerReconnect->setSingleShot(true);
        connect(masterServerReconnect, SIGNAL(timeout()), this,
                SLOT(reconnectTimeout()));
        masterServerReconnect->start(kMasterServerReconnectTimeout);
    }

    deferredDeleteTimer = new QTimer(this);
    connect(deferredDeleteTimer, SIGNAL(timeout()), this,
            SLOT(deferredDeleteSlot()));
    deferredDeleteTimer->start(30 * 1000);

    autoexpireUpdateTimer = new QTimer(this);
    connect(autoexpireUpdateTimer, SIGNAL(timeout()), this,
            SLOT(autoexpireUpdate()));
    autoexpireUpdateTimer->setSingleShot(true);

    if (sched)
        sched->SetMainServer(this);
}

MainServer::~MainServer()
{
    if (mythserver)
    {
        mythserver->disconnect();
        mythserver->deleteLater();
        mythserver = NULL;
    }
}

void MainServer::autoexpireUpdate(void)
{
    AutoExpire::Update(false);
}

void MainServer::newConnection(MythSocket *socket)
{
    socket->setCallbacks(this);
}

void MainServer::readyRead(MythSocket *sock)
{
    sockListLock.lockForRead();
    PlaybackSock *testsock = GetPlaybackBySock(sock);
    bool expecting_reply = testsock && testsock->isExpectingReply();
    sockListLock.unlock();
    if (expecting_reply)
        return;

    ProcessRequestThread *prt = NULL;
    {
        QMutexLocker locker(&threadPoolLock);

        if (threadPool.empty())
        {
            VERBOSE(VB_GENERAL, "Waiting for a process request thread.. ");
            threadPoolCond.wait(&threadPoolLock, PRT_TIMEOUT);
        }

        if (!threadPool.empty())
        {
            prt = threadPool.front();
            threadPool.pop_front();
        }
        else
        {
            VERBOSE(VB_IMPORTANT, "Adding a new process request thread");
            prt = new ProcessRequestThread(this);
            prt->lock.lock();
            prt->start();
            prt->waitCond.wait(&prt->lock);
            prt->lock.unlock();
        }
    }

    prt->setup(sock);
}

void MainServer::ProcessRequest(MythSocket *sock)
{
    sock->Lock();

    if (sock->bytesAvailable() > 0)
    {
        ProcessRequestWork(sock);
    }

    sock->Unlock();
}

void MainServer::ProcessRequestWork(MythSocket *sock)
{
    QStringList listline;
    if (!sock->readStringList(listline))
        return;

    QString line = listline[0];

    line = line.simplified();
    QStringList tokens = line.split(" ", QString::SkipEmptyParts);
    QString command = tokens[0];
    //cerr << "command='" << command << "'\n";
    if (command == "MYTH_PROTO_VERSION")
    {
        if (tokens.size() < 2)
            VERBOSE(VB_IMPORTANT, "Bad MYTH_PROTO_VERSION command");
        else
            HandleVersion(sock,tokens[1]);
        return;
    }
    else if (command == "ANN")
    {
        HandleAnnounce(listline, tokens, sock);
        return;
    }
    else if (command == "DONE")
    {
        HandleDone(sock);
        return;
    }

    sockListLock.lockForRead();
    PlaybackSock *pbs = GetPlaybackBySock(sock);
    if (!pbs)
    {
        sockListLock.unlock();
        VERBOSE(VB_IMPORTANT, "ProcessRequest unknown socket");
        return;
    }
    pbs->UpRef();
    sockListLock.unlock();

    if (command == "QUERY_RECORDINGS")
    {
        if (tokens.size() != 2)
            VERBOSE(VB_IMPORTANT, "Bad QUERY_RECORDINGS query");
        else
            HandleQueryRecordings(tokens[1], pbs);
    }
    else if (command == "QUERY_RECORDING")
    {
        HandleQueryRecording(tokens, pbs);
    }
    else if (command == "GO_TO_SLEEP")
    {
        HandleGoToSleep(pbs);
    }
    else if (command == "QUERY_FREE_SPACE")
    {
        HandleQueryFreeSpace(pbs, false);
    }
    else if (command == "QUERY_FREE_SPACE_LIST")
    {
        HandleQueryFreeSpace(pbs, true);
    }
    else if (command == "QUERY_FREE_SPACE_SUMMARY")
    {
        HandleQueryFreeSpaceSummary(pbs);
    }
    else if (command == "QUERY_LOAD")
    {
        HandleQueryLoad(pbs);
    }
    else if (command == "QUERY_UPTIME")
    {
        HandleQueryUptime(pbs);
    }
    else if (command == "QUERY_HOSTNAME")
    {
        HandleQueryHostname(pbs);
    }
    else if (command == "QUERY_MEMSTATS")
    {
        HandleQueryMemStats(pbs);
    }
    else if (command == "QUERY_TIME_ZONE")
    {
        HandleQueryTimeZone(pbs);
    }
    else if (command == "QUERY_CHECKFILE")
    {
        HandleQueryCheckFile(listline, pbs);
    }
    else if (command == "QUERY_FILE_EXISTS")
    {
        if (listline.size() < 3)
            VERBOSE(VB_IMPORTANT, "Bad QUERY_FILE_EXISTS command");
        else
            HandleQueryFileExists(listline, pbs);
    }
    else if (command == "QUERY_FILE_HASH")
    {
        if (listline.size() < 3)
            VERBOSE(VB_IMPORTANT, "Bad QUERY_FILE_HASH command");
        else
            HandleQueryFileHash(listline, pbs);
    }
    else if (command == "QUERY_GUIDEDATATHROUGH")
    {
        HandleQueryGuideDataThrough(pbs);
    }
    else if (command == "DELETE_FILE")
    {
        if (listline.size() < 3)
            VERBOSE(VB_IMPORTANT, "Bad DELETE_FILE command");
        else
            HandleDeleteFile(listline, pbs);
    }
    else if (command == "STOP_RECORDING")
    {
        HandleStopRecording(listline, pbs);
    }
    else if (command == "CHECK_RECORDING")
    {
        HandleCheckRecordingActive(listline, pbs);
    }
    else if (command == "DELETE_RECORDING")
    {
        if (3 <= tokens.size() && tokens.size() <= 4)
        {
            bool force = (tokens.size() >= 4) && (tokens[3] == "FORCE");
            HandleDeleteRecording(tokens[1], tokens[2], pbs, force);
        }
        else
            HandleDeleteRecording(listline, pbs, false);
    }
    else if (command == "FORCE_DELETE_RECORDING")
    {
        HandleDeleteRecording(listline, pbs, true);
    }
    else if (command == "UNDELETE_RECORDING")
    {
        HandleUndeleteRecording(listline, pbs);
    }
    else if (command == "RESCHEDULE_RECORDINGS")
    {
        if (tokens.size() != 2)
            VERBOSE(VB_IMPORTANT, "Bad RESCHEDULE_RECORDINGS request");
        else
            HandleRescheduleRecordings(tokens[1].toInt(), pbs);
    }
    else if (command == "FORGET_RECORDING")
    {
        HandleForgetRecording(listline, pbs);
    }
    else if (command == "QUERY_GETALLPENDING")
    {
        if (tokens.size() == 1)
            HandleGetPendingRecordings(pbs);
        else if (tokens.size() == 2)
            HandleGetPendingRecordings(pbs, tokens[1]);
        else
            HandleGetPendingRecordings(pbs, tokens[1], tokens[2].toInt());
    }
    else if (command == "QUERY_GETALLSCHEDULED")
    {
        HandleGetScheduledRecordings(pbs);
    }
    else if (command == "QUERY_GETCONFLICTING")
    {
        HandleGetConflictingRecordings(listline, pbs);
    }
    else if (command == "QUERY_GETEXPIRING")
    {
        HandleGetExpiringRecordings(pbs);
    }
    else if (command == "QUERY_SG_GETFILELIST")
    {
        HandleSGGetFileList(listline, pbs);
    }
    else if (command == "QUERY_SG_FILEQUERY")
    {   
        HandleSGFileQuery(listline, pbs);
    }
    else if (command == "GET_FREE_RECORDER")
    {
        HandleGetFreeRecorder(pbs);
    }
    else if (command == "GET_FREE_RECORDER_COUNT")
    {
        HandleGetFreeRecorderCount(pbs);
    }
    else if (command == "GET_FREE_RECORDER_LIST")
    {
        HandleGetFreeRecorderList(pbs);
    }
    else if (command == "GET_NEXT_FREE_RECORDER")
    {
        HandleGetNextFreeRecorder(listline, pbs);
    }
    else if (command == "QUERY_RECORDER")
    {
        if (tokens.size() != 2)
            VERBOSE(VB_IMPORTANT, "Bad QUERY_RECORDER");
        else
            HandleRecorderQuery(listline, tokens, pbs);
    }
    else if (command == "SET_NEXT_LIVETV_DIR")
    {
        if (tokens.size() != 3)
            VERBOSE(VB_IMPORTANT, "Bad SET_NEXT_LIVETV_DIR");
        else
            HandleSetNextLiveTVDir(tokens, pbs);
    }
    else if (command == "SET_CHANNEL_INFO")
    {
        HandleSetChannelInfo(listline, pbs);
    }
    else if (command == "QUERY_REMOTEENCODER")
    {
        if (tokens.size() != 2)
            VERBOSE(VB_IMPORTANT, "Bad QUERY_REMOTEENCODER");
        else
            HandleRemoteEncoder(listline, tokens, pbs);
    }
    else if (command == "GET_RECORDER_FROM_NUM")
    {
        HandleGetRecorderFromNum(listline, pbs);
    }
    else if (command == "GET_RECORDER_NUM")
    {
        HandleGetRecorderNum(listline, pbs);
    }
    else if (command == "QUERY_FILETRANSFER")
    {
        if (tokens.size() != 2)
            VERBOSE(VB_IMPORTANT, "Bad QUERY_FILETRANSFER");
        else
            HandleFileTransferQuery(listline, tokens, pbs);
    }
    else if (command == "QUERY_GENPIXMAP")
    {
        HandleGenPreviewPixmap(listline, pbs);
    }
    else if (command == "QUERY_PIXMAP_LASTMODIFIED")
    {
        HandlePixmapLastModified(listline, pbs);
    }
    else if (command == "QUERY_PIXMAP_GET_IF_MODIFIED")
    {
        HandlePixmapGetIfModified(listline, pbs);
    }
    else if (command == "QUERY_ISRECORDING")
    {
        HandleIsRecording(listline, pbs);
    }
    else if (command == "MESSAGE")
    {
        if ((listline.size() >= 2) && (listline[1].left(11) == "SET_VERBOSE"))
            HandleSetVerbose(listline, pbs);
        else
            HandleMessage(listline, pbs);
    }
    else if (command == "FILL_PROGRAM_INFO")
    {
        HandleFillProgramInfo(listline, pbs);
    }
    else if (command == "LOCK_TUNER")
    {
        if (tokens.size() == 1)
            HandleLockTuner(pbs);
        else if (tokens.size() == 2)
            HandleLockTuner(pbs, tokens[1].toInt());
        else
            VERBOSE(VB_IMPORTANT, "Bad LOCK_TUNER query");
    }
    else if (command == "FREE_TUNER")
    {
        if (tokens.size() != 2)
            VERBOSE(VB_IMPORTANT, "Bad FREE_TUNER query");
        else
            HandleFreeTuner(tokens[1].toInt(), pbs);
    }
    else if (command == "QUERY_IS_ACTIVE_BACKEND")
    {
        if (tokens.size() != 1)
            VERBOSE(VB_IMPORTANT, "Bad QUERY_IS_ACTIVE_BACKEND");
        else
            HandleIsActiveBackendQuery(listline, pbs);
    }
    else if (command == "QUERY_COMMBREAK")
    {
        if (tokens.size() != 3)
            VERBOSE(VB_IMPORTANT, "Bad QUERY_COMMBREAK");
        else
            HandleCommBreakQuery(tokens[1], tokens[2], pbs);
    }
    else if (command == "QUERY_CUTLIST")
    {
        if (tokens.size() != 3)
            VERBOSE(VB_IMPORTANT, "Bad QUERY_CUTLIST");
        else
            HandleCutlistQuery(tokens[1], tokens[2], pbs);
    }
    else if (command == "QUERY_BOOKMARK")
    {
        if (tokens.size() != 3)
            VERBOSE(VB_IMPORTANT, "Bad QUERY_BOOKMARK");
        else
            HandleBookmarkQuery(tokens[1], tokens[2], pbs);
    }
    else if (command == "SET_BOOKMARK")
    {
        if (tokens.size() != 5)
            VERBOSE(VB_IMPORTANT, "Bad SET_BOOKMARK");
        else
            HandleSetBookmark(tokens, pbs);
    }
    else if (command == "QUERY_SETTING")
    {
        if (tokens.size() != 3)
            VERBOSE(VB_IMPORTANT, "Bad QUERY_SETTING");
        else
            HandleSettingQuery(tokens, pbs);
    }
    else if (command == "SET_SETTING")
    {
        if (tokens.size() != 4)
            VERBOSE(VB_IMPORTANT, "Bad SET_SETTING");
        else
            HandleSetSetting(tokens, pbs);
    }
    else if (command == "ALLOW_SHUTDOWN")
    {
        if (tokens.size() != 1)
            VERBOSE(VB_IMPORTANT, "Bad ALLOW_SHUTDOWN");
        else
            HandleBlockShutdown(false, pbs);
    }
    else if (command == "BLOCK_SHUTDOWN")
    {
        if (tokens.size() != 1)
            VERBOSE(VB_IMPORTANT, "Bad BLOCK_SHUTDOWN");
        else
            HandleBlockShutdown(true, pbs);
    }
    else if (command == "SHUTDOWN_NOW")
    {
        if (tokens.size() != 1)
            VERBOSE(VB_IMPORTANT, "Bad SHUTDOWN_NOW query");
        else if (!ismaster)
        {
            QString halt_cmd;
            if (listline.size() >= 2)
                halt_cmd = listline[1];

            if (!halt_cmd.isEmpty())
            {
                VERBOSE(VB_IMPORTANT,
                        "Going down now as of Mainserver request!");
                myth_system(halt_cmd);
            }
            else
                VERBOSE(VB_IMPORTANT,
                        "WARNING: Received an empty SHUTDOWN_NOW query!");
        }
    }
    else if (command == "BACKEND_MESSAGE")
    {
        QString message = listline[1];
        QStringList extra( listline[2] );
        for (int i = 3; i < listline.size(); i++)
            extra << listline[i];
        MythEvent me(message, extra);
        gContext->dispatch(me);
    }
    else if (command == "REFRESH_BACKEND")
    {
        VERBOSE(VB_IMPORTANT,"Reloading backend settings");
        HandleBackendRefresh(sock);
    }
    else if (command == "OK")
    {
        VERBOSE(VB_IMPORTANT, "Got 'OK' out of sequence.");
    }
    else if (command == "UNKNOWN_COMMAND")
    {
        VERBOSE(VB_IMPORTANT, "Got 'UNKNOWN_COMMAND' out of sequence.");
    }
    else
    {
        VERBOSE(VB_IMPORTANT, "Unknown command: " + command);

        MythSocket *pbssock = pbs->getSocket();

        QStringList strlist;
        strlist << "UNKNOWN_COMMAND";

        SendResponse(pbssock, strlist);
    }

    // Decrease refcount..
    pbs->DownRef();
}

void MainServer::MarkUnused(ProcessRequestThread *prt)
{
    QMutexLocker locker(&threadPoolLock);
    threadPool.push_back(prt);
    threadPoolCond.wakeAll();
}

void MainServer::customEvent(QEvent *e)
{
    QStringList broadcast;

    if ((MythEvent::Type)(e->type()) == MythEvent::MythEventMessage)
    {
        MythEvent *me = (MythEvent *)e;

        if (me->Message().left(11) == "AUTO_EXPIRE")
        {
            QStringList tokens = me->Message()
                .split(" ", QString::SkipEmptyParts);

            if (tokens.size() != 3)
            {
                VERBOSE(VB_IMPORTANT, "Bad AUTO_EXPIRE message");
                return;
            }

            QDateTime startts = QDateTime::fromString(tokens[2], Qt::ISODate);
            ProgramInfo *pinfo = ProgramInfo::GetProgramFromRecorded(tokens[1],
                                                                     startts);
            if (pinfo)
            {
                SendMythSystemPlayEvent("REC_EXPIRED", pinfo);

                RecordingInfo recInfo(*pinfo);
                delete pinfo;

                // allow re-record if auto expired but not expired live
                // or already "deleted" programs
                if (recInfo.recgroup != "LiveTV" &&
                    recInfo.recgroup != "Deleted" &&
                    (gContext->GetNumSetting("RerecordWatched", 0) ||
                     !(recInfo.GetProgramFlags() & FL_WATCHED)))
                {
                    recInfo.ForgetHistory();
                }
                DoHandleDeleteRecording(recInfo, NULL, false, true);
            }
            else
            {
                QString msg = QString("Cannot find program info for '%1', "
                                      "while attempting to Auto-Expire.")
                    .arg(me->Message());
                cerr << msg.toLocal8Bit().constData() << endl;
            }

            return;
        }

        if (me->Message().left(21) == "QUERY_NEXT_LIVETV_DIR" && m_sched)
        {
            QStringList tokens = me->Message()
                .split(" ", QString::SkipEmptyParts);

            if (tokens.size() != 2)
            {
                VERBOSE(VB_IMPORTANT, QString("Bad %1 message").arg(tokens[0]));
                return;
            }

            m_sched->GetNextLiveTVDir(tokens[1].toInt());
            return;
        }

        if ((me->Message().left(16) == "DELETE_RECORDING") ||
            (me->Message().left(22) == "FORCE_DELETE_RECORDING"))
        {
            QStringList tokens = me->Message()
                .split(" ", QString::SkipEmptyParts);

            if (tokens.size() != 3)
            {
                VERBOSE(VB_IMPORTANT, QString("Bad %1 message").arg(tokens[0]));
                return;
            }

            QDateTime startts = QDateTime::fromString(tokens[2], Qt::ISODate);
            ProgramInfo *pinfo = ProgramInfo::GetProgramFromRecorded(tokens[1],
                                                                     startts);
            if (pinfo)
            {
                RecordingInfo recInfo(*pinfo);
                delete pinfo;
                if (tokens[0] == "FORCE_DELETE_RECORDING")
                    DoHandleDeleteRecording(recInfo, NULL, true);
                else
                    DoHandleDeleteRecording(recInfo, NULL, false);
            }
            else
            {
                VERBOSE(VB_IMPORTANT,
                    QString("Cannot find program info for '%1' while "
                            "attempting to delete.").arg(me->Message()));
            }

            return;
        }

        if (me->Message().left(21) == "RESCHEDULE_RECORDINGS" && m_sched)
        {
            QStringList tokens = me->Message()
                .split(" ", QString::SkipEmptyParts);

            if (tokens.size() != 2)
            {
                VERBOSE(VB_IMPORTANT, "Bad RESCHEDULE_RECORDINGS message");
                return;
            }

            int recordid = tokens[1].toInt();
            m_sched->Reschedule(recordid);
            return;
        }

        if (me->Message().left(23) == "SCHEDULER_ADD_RECORDING" && m_sched)
        {
            ProgramInfo pi;
            QStringList list = me->ExtraDataList();
            if (!pi.FromStringList(list, 0))
            {
                VERBOSE(VB_IMPORTANT, "Bad SCHEDULER_ADD_RECORDING message");
                return;
            }

            m_sched->AddRecording(pi);
            return;
        }

        if (me->Message().left(23) == "UPDATE_RECORDING_STATUS" && m_sched)
        {
            QStringList tokens = me->Message()
                .split(" ", QString::SkipEmptyParts);

            if (tokens.size() != 6)
            {
                VERBOSE(VB_IMPORTANT, "Bad UPDATE_RECORDING_STATUS message");
                return;
            }

            int cardid = tokens[1].toInt();
            QString chanid = tokens[2];
            QDateTime startts = QDateTime::fromString(tokens[3], Qt::ISODate);
            RecStatusType recstatus = RecStatusType(tokens[4].toInt());
            QDateTime recendts = QDateTime::fromString(tokens[5], Qt::ISODate);
            m_sched->UpdateRecStatus(cardid, chanid, startts,
                                     recstatus, recendts);
            return;
        }

        if (me->Message().left(13) == "LIVETV_EXITED")
        {
            QString chainid = me->ExtraData();
            LiveTVChain *chain = GetExistingChain(chainid);
            if (chain)
                DeleteChain(chain);

            return;
        }

        if (me->Message() == "CLEAR_SETTINGS_CACHE")
            gContext->ClearSettingsCache();

        if (me->Message().left(14) == "RESET_IDLETIME" && m_sched)
            m_sched->ResetIdleTime();

        if (me->Message() == "LOCAL_RECONNECT_TO_MASTER")
            masterServerReconnect->start(kMasterServerReconnectTimeout);

        if (me->Message().left(6) == "LOCAL_")
            return;

        MythEvent mod_me("");
        if (me->Message().left(23) == "MASTER_UPDATE_PROG_INFO")
        {
            QStringList tokens = me->Message().simplified().split(" ");
            uint chanid = 0;
            QDateTime recstartts;
            if (tokens.size() >= 3)
            {
                chanid     = tokens[1].toUInt();
                recstartts = QDateTime::fromString(tokens[2], Qt::ISODate);
            }

            ProgramInfo evinfo;
            if (chanid && recstartts.isValid() &&
                evinfo.LoadProgramFromRecorded(chanid, recstartts))
            {
                QDateTime rectime = QDateTime::currentDateTime().addSecs(
                    -gContext->GetNumSetting("RecordOverTime"));

                if (m_sched && evinfo.recendts > rectime)
                    evinfo.recstatus = m_sched->GetRecStatus(evinfo);

                QStringList list;
                evinfo.ToStringList(list);
                mod_me = MythEvent("RECORDING_LIST_CHANGE UPDATE", list);
                me = &mod_me;
            }
            else
            {
                return;
            }
        }

        broadcast = QStringList( "BACKEND_MESSAGE" );
        broadcast << me->Message();
        broadcast += me->ExtraDataList();
    }

    if (!broadcast.empty())
    {
        // Make a local copy of the list, upping the refcount as we go..
        vector<PlaybackSock *> localPBSList;
        sockListLock.lockForRead();
        vector<PlaybackSock *>::iterator it = playbackList.begin();
        for (; it != playbackList.end(); ++it)
        {
            (*it)->UpRef();
            localPBSList.push_back(*it);
        }
        sockListLock.unlock();

        bool sendGlobal = false;
        if (ismaster && broadcast[1].left(7) == "GLOBAL_")
        {
            broadcast[1].replace("GLOBAL_", "LOCAL_");
            MythEvent me(broadcast[1], broadcast[2]);
            gContext->dispatch(me);

            sendGlobal = true;
        }

        vector<PlaybackSock*> sentSet;

        bool isSystemEvent = broadcast[1].startsWith("SYSTEM_EVENT ");
        QStringList sentSetSystemEvent(gContext->GetHostName());

        vector<PlaybackSock*>::const_iterator iter;
        for (iter = localPBSList.begin(); iter != localPBSList.end(); iter++)
        {
            PlaybackSock *pbs = *iter;

            vector<PlaybackSock*>::const_iterator it =
                find(sentSet.begin(), sentSet.end(), pbs);
            if (it != sentSet.end() || pbs->IsDisconnected())
                continue;

            sentSet.push_back(pbs);

            bool reallysendit = false;

            if (broadcast[1] == "CLEAR_SETTINGS_CACHE")
            {
                if ((ismaster) &&
                    (pbs->isSlaveBackend() || pbs->wantsEvents()))
                    reallysendit = true;
            }
            else if (sendGlobal)
            {
                if (pbs->isSlaveBackend())
                    reallysendit = true;
            }
            else if (pbs->wantsEvents())
            {
                reallysendit = true;
            }

            if (reallysendit)
            {
                if (isSystemEvent)
                {
                    if (!pbs->wantsSystemEvents())
                    {
                        continue;
                    }
                    else if (!pbs->wantsOnlySystemEvents())
                    {
                        if (sentSetSystemEvent.contains(pbs->getHostname()))
                            continue;

                        sentSetSystemEvent << pbs->getHostname();
                    }
                }
                else if (pbs->wantsOnlySystemEvents())
                    continue;
            }

            MythSocket *sock = pbs->getSocket();
            if (reallysendit && sock->socket() >= 0)
            {
                sock->Lock();

                if (sock->socket() >= 0)
                    sock->writeStringList(broadcast);

                sock->Unlock();
            }
        }

        // Done with the pbs list, so decrement all the instances..
        for (iter = localPBSList.begin(); iter != localPBSList.end(); iter++)
        {
            PlaybackSock *pbs = *iter;
            pbs->DownRef();
        }
    }
}

/**
 * \addtogroup myth_network_protocol
 * \par        MYTH_PROTO_VERSION \e version
 * Checks that \e version matches the backend's version.
 * If it matches, the stringlist of "ACCEPT" \e "version" is returned.
 * If it does not, "REJECT" \e "version" is returned,
 * and the socket is closed (for this client)
 */
void MainServer::HandleVersion(MythSocket *socket, QString version)
{
    QStringList retlist;
    if (version != MYTH_PROTO_VERSION)
    {
        VERBOSE(VB_GENERAL,
                "MainServer::HandleVersion - Client speaks protocol version "
                + version + " but we speak " + MYTH_PROTO_VERSION + "!");
        retlist << "REJECT" << MYTH_PROTO_VERSION;
        socket->writeStringList(retlist);
        HandleDone(socket);
        return;
    }

    retlist << "ACCEPT" << MYTH_PROTO_VERSION;
    socket->writeStringList(retlist);
}

/**
 * \addtogroup myth_network_protocol
 * \par        ANN Playback \e host \e wantevents
 * Register \e host as a client, and prevent shutdown of the socket.
 *
 * \par        ANN Monitor  \e host \e wantevents
 * Register \e host as a client, and allow shutdown of the socket
 * \par        ANN SlaveBackend \e IPaddress
 * \par        ANN FileTransfer stringlist(\e hostname, \e filename)
 * \par        ANN FileTransfer stringlist(\e hostname, \e filename) \e useReadahead \e retries
 */
void MainServer::HandleAnnounce(QStringList &slist, QStringList commands,
                                MythSocket *socket)
{
    QStringList retlist( "OK" );
    QStringList errlist( "ERROR" );

    if (commands.size() < 3 || commands.size() > 6)
    {
        QString info = "";
        if (commands.size() == 2)
            info = QString(" %1").arg(commands[1]);

        VERBOSE(VB_IMPORTANT,
                QString("Received malformed ANN%1 query")
                .arg(info));

        errlist << "malformed_ann_query";
        socket->writeStringList(errlist);
        return;
    }

    sockListLock.lockForRead();
    vector<PlaybackSock *>::iterator iter = playbackList.begin();
    for (; iter != playbackList.end(); iter++)
    {
        PlaybackSock *pbs = *iter;
        if (pbs->getSocket() == socket)
        {
            VERBOSE(VB_IMPORTANT,
                    QString("Client %1 is trying to announce a socket "
                            "multiple times.")
                    .arg(commands[2]));
            socket->writeStringList(retlist);
            sockListLock.unlock();
            return;
        }
    }
    sockListLock.unlock();

    if (commands[1] == "Playback" || commands[1] == "Monitor")
    {
        if (commands.size() < 4)
        {
            VERBOSE(VB_IMPORTANT,
                    QString("Received malformed ANN %1 query")
                    .arg(commands[1]));

            errlist << "malformed_ann_query";
            socket->writeStringList(errlist);
            return;
        }
        // Monitor connections are same as Playback but they don't
        // block shutdowns. See the Scheduler event loop for more.

        PlaybackSockEventsMode eventsMode =
            (PlaybackSockEventsMode)commands[3].toInt();
        VERBOSE(VB_GENERAL, QString("MainServer::ANN %1")
                                    .arg(commands[1]));
        VERBOSE(VB_IMPORTANT, QString("adding: %1 as a client (events: %2)")
                               .arg(commands[2]).arg(eventsMode));
        PlaybackSock *pbs = new PlaybackSock(this, socket, commands[2], eventsMode);
        pbs->setBlockShutdown(commands[1] == "Playback");

        sockListLock.lockForWrite();
        playbackList.push_back(pbs);
        sockListLock.unlock();

        if (eventsMode != kPBSEvents_None && commands[2] != "tzcheck")
            SendMythSystemEvent(QString("CLIENT_CONNECTED HOSTNAME %1")
                                        .arg(commands[2]));
    }
    else if (commands[1] == "SlaveBackend")
    {
        if (commands.size() < 4)
        {
            VERBOSE(VB_IMPORTANT, QString("Received malformed ANN %1 query")
                    .arg(commands[1]));
            errlist << "malformed_ann_query";
            socket->writeStringList(errlist);
            return;
        }

        VERBOSE(VB_IMPORTANT, QString("adding: %1 as a slave backend server")
                               .arg(commands[2]));
        PlaybackSock *pbs = new PlaybackSock(this, socket, commands[2],
                                             kPBSEvents_None);
        pbs->setAsSlaveBackend();
        pbs->setIP(commands[3]);

        if (m_sched)
        {
            RecordingInfo pinfo;
            RecordingList slavelist;
            QStringList::const_iterator sit = slist.begin()+1;
            while (sit != slist.end())
            {
                if (!pinfo.FromStringList(sit, slist.end()))
                    break;
                slavelist.push_back(new RecordingInfo(pinfo));
            }
            m_sched->SlaveConnected(slavelist);
        }

        bool wasAsleep = true;
        QMap<int, EncoderLink *>::Iterator iter = encoderList->begin();
        for (; iter != encoderList->end(); ++iter)
        {
            EncoderLink *elink = *iter;
            if (elink->GetHostName() == commands[2])
            {
                if (! (elink->IsWaking() || elink->IsAsleep()))
                    wasAsleep = false;
                elink->SetSocket(pbs);
            }
        }

        if (!wasAsleep && m_sched)
            m_sched->Reschedule(0);

        QString message = QString("LOCAL_SLAVE_BACKEND_ONLINE %2")
                                  .arg(commands[2]);
        MythEvent me(message);
        gContext->dispatch(me);

        pbs->setBlockShutdown(false);

        sockListLock.lockForWrite();
        playbackList.push_back(pbs);
        sockListLock.unlock();

        autoexpireUpdateTimer->start(1000);

        SendMythSystemEvent(QString("SLAVE_CONNECTED HOSTNAME %1")
                                    .arg(commands[2]));
    }
    else if (commands[1] == "FileTransfer")
    {
        if (slist.size() < 3)
        {
            VERBOSE(VB_IMPORTANT, "Received malformed FileTransfer command");
            errlist << "malformed_filetransfer_command";
            socket->writeStringList(errlist);
            return;
        }

        VERBOSE(VB_GENERAL, "MainServer::HandleAnnounce FileTransfer");
        VERBOSE(VB_IMPORTANT, QString("adding: %1 as a remote file transfer")
                               .arg(commands[2]));
        QStringList::const_iterator it = slist.begin();
        QUrl qurl = *(++it);
        QString wantgroup = *(++it);
        QString filename;
        QStringList checkfiles;
        for (++it; it != slist.end(); ++it)
            checkfiles += *it;

        FileTransfer *ft = NULL;
        bool writemode = false;
        bool usereadahead = true;
        int retries = -1;
        if (commands.size() > 3)
            writemode = commands[3].toInt();

        if (commands.size() > 4)
            usereadahead = commands[4].toInt();

        if (commands.size() > 5)
            retries = commands[5].toInt();

        if (writemode)
        {
            if (wantgroup.isEmpty())
                wantgroup = "Default";

            StorageGroup sgroup(wantgroup, gContext->GetHostName(), false);
            QString dir = sgroup.FindNextDirMostFree();
            if (dir.isEmpty())
            {
                VERBOSE(VB_IMPORTANT, "Unable to determine directory "
                        "to write to in FileTransfer write command");
                errlist << "filetransfer_directory_not_found";
                socket->writeStringList(errlist);
                return;
            }

            QString basename = qurl.path();
            if (basename.isEmpty())
            {
                VERBOSE(VB_IMPORTANT, QString("ERROR: FileTransfer write "
                        "filename is empty in url '%1'.")
                        .arg(qurl.toString()));
                errlist << "filetransfer_filename_empty";
                socket->writeStringList(errlist);
                return;
            }

            if ((basename.contains("/../")) ||
                (basename.startsWith("../")))
            {
                VERBOSE(VB_IMPORTANT, QString("ERROR: FileTransfer write "
                        "filename '%1' does not pass sanity checks.")
                        .arg(basename));
                errlist << "filetransfer_filename_dangerous";
                socket->writeStringList(errlist);
                return;
            }

            filename = dir + "/" + basename;
        }
        else
            filename = LocalFilePath(qurl, wantgroup);

        if (writemode)
        {
            socket->setCallbacks(NULL);
            ft = new FileTransfer(filename, socket, true);
        }
        else if (retries >= 0)
            ft = new FileTransfer(filename, socket, usereadahead, retries);
        else
            ft = new FileTransfer(filename, socket, false);

        sockListLock.lockForWrite();
        fileTransferList.push_back(ft);
        sockListLock.unlock();

        retlist << QString::number(socket->socket());
        ft->UpRef();
        encodeLongLong(retlist, ft->GetFileSize());
        ft->DownRef();

        if (checkfiles.size())
        {
            QFileInfo fi(filename);
            QDir dir = fi.absoluteDir();
            for (it = checkfiles.begin(); it != checkfiles.end(); ++it)
            {
                if (dir.exists(*it) &&
                    QFileInfo(dir, *it).size() >= RingBuffer::kReadTestSize)
                {
                    retlist<<*it;
                }
            }
        }
    }

    socket->writeStringList(retlist);
}

/**
 * \addtogroup myth_network_protocol
 * \par        DONE
 * Closes this client's socket.
 */
void MainServer::HandleDone(MythSocket *socket)
{
    socket->close();
}

void MainServer::SendResponse(MythSocket *socket, QStringList &commands)
{
    // Note: this method assumes that the playback or filetransfer
    // handler has already been uprefed and the socket as well.

    // These checks are really just to check if the socket has
    // been remotely disconnected while we were working on the
    // response.

    bool do_write = false;
    if (socket)
    {
        sockListLock.lockForRead();
        do_write = (GetPlaybackBySock(socket) ||
                    GetFileTransferBySock(socket));
        sockListLock.unlock();
    }

    if (do_write)
    {
        socket->writeStringList(commands);
    }
    else
    {
        VERBOSE(VB_IMPORTANT, "SendResponse: Unable to write to client socket,"
                " as it's no longer there");
    }
}

/**
 * \addtogroup myth_network_protocol
 * \par        QUERY_RECORDINGS \e type
 * The \e type parameter can be either "Play", "Recording" or "Delete".
 * Returns programinfo (title, subtitle, description, category, chanid,
 * channum, callsign, channel.name, fileURL, \e et \e cetera)
 */
void MainServer::HandleQueryRecordings(QString type, PlaybackSock *pbs)
{
    MythSocket *pbssock = pbs->getSocket();
    QString playbackhost = pbs->getHostname();

    RecList rlSchedList;
    if (m_sched)
        m_sched->getAllPending(&rlSchedList);
    ProgramList schedList;
    while (!rlSchedList.empty())
    {
        schedList.push_front(rlSchedList.front());
        rlSchedList.pop_front();
    }

    ProgramList destination;
    LoadFromRecorded(
        destination, (type == "Delete"), (type == "Recording"), schedList);

    QStringList outputlist(QString::number(destination.size()));
    QMap<QString, QString> backendIpMap;
    QMap<QString, QString> backendPortMap;
    QString ip   = gContext->GetSetting("BackendServerIP");
    QString port = gContext->GetSetting("BackendServerPort");

    ProgramList::iterator it = destination.begin();
    for (it = destination.begin(); it != destination.end(); ++it)
    {
        ProgramInfo *proginfo = *it;
        PlaybackSock *slave = NULL;

        if (proginfo->hostname != gContext->GetHostName())
            slave = getSlaveByHostname(proginfo->hostname);

        if ((proginfo->hostname == gContext->GetHostName()) ||
            (!slave && masterBackendOverride))
        {
            proginfo->pathname = QString("myth://") + ip + ":" + port
                + "/" + proginfo->pathname;
            if (proginfo->filesize == 0)
            {
                QString tmpURL = GetPlaybackURL(proginfo);
                if (tmpURL.startsWith("/"))
                {
                    QFile checkFile(tmpURL);
                    if (!tmpURL.isEmpty() && checkFile.exists())
                    {
                        proginfo->filesize = checkFile.size();
                        if (proginfo->recendts < QDateTime::currentDateTime())
                            proginfo->SetFilesize(proginfo->filesize);
                    }
                }
            }
        }
        else if (!slave)
        {
            proginfo->pathname = GetPlaybackURL(proginfo);
            if (proginfo->pathname.isEmpty())
            {
                VERBOSE(VB_IMPORTANT,
                        "MainServer::HandleQueryRecordings()"
                        "\n\t\t\tCouldn't find backend for: " +
                        QString("\n\t\t\t%1 : \"%2\"")
                        .arg(proginfo->title).arg(proginfo->subtitle));

                proginfo->filesize = 0;
                proginfo->pathname = "file not found";
            }
        }
        else
        {
            if (proginfo->filesize == 0)
            {
                if (!slave->FillProgramInfo(proginfo, playbackhost))
                {
                    VERBOSE(VB_IMPORTANT,
                            "MainServer::HandleQueryRecordings()"
                            "\n\t\t\tCould not fill program info "
                            "from backend");
                }
                else
                {
                    if (proginfo->recendts < QDateTime::currentDateTime())
                        proginfo->SetFilesize(proginfo->filesize);
                }
            }
            else
            {
                ProgramInfo *p = proginfo;
                if (!backendIpMap.contains(p->hostname))
                    backendIpMap[p->hostname] =
                        gContext->GetSettingOnHost("BackendServerIp",
                                                   p->hostname);
                if (!backendPortMap.contains(p->hostname))
                    backendPortMap[p->hostname] =
                        gContext->GetSettingOnHost("BackendServerPort",
                                                   p->hostname);
                p->pathname = QString("myth://") +
                    backendIpMap[p->hostname] + ":" +
                    backendPortMap[p->hostname] + "/" +
                    p->pathname;
            }
        }

        if (slave)
            slave->DownRef();

        proginfo->ToStringList(outputlist);
    }

    SendResponse(pbssock, outputlist);
}

/**
 * \addtogroup myth_network_protocol
 * \par        QUERY_RECORDING BASENAME \e basename
 * \par        QUERY_RECORDING TIMESLOT \e chanid \e starttime
 */
void MainServer::HandleQueryRecording(QStringList &slist, PlaybackSock *pbs)
{
    if (slist.size() < 3)
    {
        VERBOSE(VB_IMPORTANT, "Bad QUERY_RECORDING query");
        return;
    }

    MythSocket *pbssock = pbs->getSocket();
    QString command = slist[1].toUpper();
    ProgramInfo *pginfo = NULL;

    if (command == "BASENAME")
    {
        pginfo = ProgramInfo::GetProgramFromBasename(slist[2]);
    }
    else if (command == "TIMESLOT")
    {
        if (slist.size() < 4)
        {
            VERBOSE(VB_IMPORTANT, "Bad QUERY_RECORDING query");
            return;
        }

        pginfo = ProgramInfo::GetProgramFromRecorded(slist[2], slist[3]);
    }

    QStringList strlist;

    if (pginfo)
    {
        strlist << "OK";
        pginfo->ToStringList(strlist);
        delete pginfo;
    }
    else
    {
        strlist << "ERROR";
    }

    SendResponse(pbssock, strlist);
}

void MainServer::HandleFillProgramInfo(QStringList &slist, PlaybackSock *pbs)
{
    MythSocket *pbssock = pbs->getSocket();

    QString playbackhost = slist[1];

    ProgramInfo *pginfo = new ProgramInfo();
    pginfo->FromStringList(slist, 2);

    QString lpath = GetPlaybackURL(pginfo);
    QString ip = gContext->GetSetting("BackendServerIP");
    QString port = gContext->GetSetting("BackendServerPort");

    if (playbackhost == gContext->GetHostName())
        pginfo->pathname = lpath;
    else
        pginfo->pathname = QString("myth://") + ip + ":" + port
                           + "/" + pginfo->GetRecordBasename();

    const QFileInfo info(lpath);
    pginfo->filesize = info.size();

    QStringList strlist;

    pginfo->ToStringList(strlist);

    delete pginfo;

    SendResponse(pbssock, strlist);
}

void *MainServer::SpawnDeleteThread(void *param)
{
    DeleteStruct *ds = (DeleteStruct *)param;

    MainServer *ms = ds->ms;
    ms->DoDeleteThread(ds);

    delete ds;

    return NULL;
}

void MainServer::DoDeleteThread(const DeleteStruct *ds)
{
    // sleep a little to let frontends reload the recordings list
    // after deleting a recording, then we can hammer the DB and filesystem
    sleep(3);
    usleep(rand()%2000);

    deletelock.lock();

    QString logInfo = QString("chanid %1 at %2")
                              .arg(ds->chanid).arg(ds->recstartts.toString());

    QString name = QString("deleteThread%1%2").arg(getpid()).arg(rand());
    QFile checkFile(ds->filename);

    if (!MSqlQuery::testDBConnection())
    {
        QString msg = QString("ERROR opening database connection for Delete "
                              "Thread for chanid %1 recorded at %2.  Program "
                              "will NOT be deleted.")
                              .arg(ds->chanid).arg(ds->recstartts.toString());
        VERBOSE(VB_GENERAL, msg);
        gContext->LogEntry("mythbackend", LP_ERROR, "Delete Recording",
                           QString("Unable to open database connection for %1. "
                                   "Program will NOT be deleted.")
                                   .arg(logInfo));

        deletelock.unlock();
        return;
    }

    auto_ptr<ProgramInfo> pginfo(
            ProgramInfo::GetProgramFromRecorded(ds->chanid, ds->recstartts));
    if (pginfo.get() == NULL)
    {
        QString msg = QString("ERROR retrieving program info when trying to "
                              "delete program for chanid %1 recorded at %2. "
                              "Recording will NOT be deleted.")
                              .arg(ds->chanid).arg(ds->recstartts.toString());
        VERBOSE(VB_GENERAL, msg);
        gContext->LogEntry("mythbackend", LP_ERROR, "Delete Recording",
                           QString("Unable to retrieve program info for %1. "
                                   "Program will NOT be deleted.")
                                   .arg(logInfo));

        deletelock.unlock();
        return;
    }

    // allow deleting files where the recording failed (ie, filesize == 0)
    if ((!checkFile.exists()) &&
        (pginfo->filesize > 0) &&
        (!ds->forceMetadataDelete))
    {
        VERBOSE(VB_IMPORTANT, QString(
                    "ERROR when trying to delete file: %1. File "
                    "doesn't exist.  Database metadata"
                    "will not be removed.")
                .arg(ds->filename));
        gContext->LogEntry("mythbackend", LP_WARNING, "Delete Recording",
                           QString("File %1 does not exist for %2 when trying "
                                   "to delete recording.")
                           .arg(ds->filename).arg(logInfo));

        pginfo->SetDeleteFlag(false);
        deletelock.unlock();
        return;
    }

    JobQueue::DeleteAllJobs(ds->chanid, ds->recstartts);

    LiveTVChain *tvchain = GetChainWithRecording(*pginfo.get());
    if (tvchain)
        tvchain->DeleteProgram(pginfo.get());

    bool followLinks = gContext->GetNumSetting("DeletesFollowLinks", 0);
    bool slowDeletes = gContext->GetNumSetting("TruncateDeletesSlowly", 0);
    int fd = -1;
    off_t size = 0;
    bool errmsg = false;

    /* Delete recording. */
    if (slowDeletes)
    {
        // Since stat fails after unlinking on some filesystems,
        // get the filesize first
        const QFileInfo info(ds->filename);
        size = info.size();
        fd = DeleteFile(ds->filename, followLinks, ds->forceMetadataDelete);

        if ((fd < 0) && checkFile.exists())
            errmsg = true;
    }
    else
    {
        delete_file_immediately(ds->filename, followLinks, false);
        sleep(2);
        if (checkFile.exists())
            errmsg = true;
    }

    if (errmsg)
    {
        VERBOSE(VB_IMPORTANT,
            QString("Error deleting file: %1. Keeping metadata in database.")
                    .arg(ds->filename));
        gContext->LogEntry("mythbackend", LP_WARNING, "Delete Recording",
                           QString("File %1 for %2 could not be deleted.")
                                   .arg(ds->filename).arg(logInfo));

        pginfo->SetDeleteFlag(false);
        deletelock.unlock();
        return;
    }

    /* Delete all preview thumbnails. */

    QFileInfo fInfo( ds->filename );
    QString nameFilter = fInfo.fileName() + "*.png";
    // QDir's nameFilter uses spaces or semicolons to separate globs,
    // so replace them with the "match any character" wildcard
    // since mythrename.pl may have included them in filenames
    nameFilter.replace(QRegExp("( |;)"), "?");
    QDir      dir  ( fInfo.path(), nameFilter );

    for (uint nIdx = 0; nIdx < dir.count(); nIdx++)
    {
        QString sFileName = QString( "%1/%2" )
                               .arg( fInfo.path() )
                               .arg( dir[ nIdx ] );

        delete_file_immediately( sFileName, followLinks, true);
    }

    DeleteRecordedFiles(ds);

    DoDeleteInDB(ds);

    if (pginfo->recgroup != "LiveTV")
        ScheduledRecording::signalChange(0);

    deletelock.unlock();

    if (slowDeletes && fd >= 0)
        TruncateAndClose(pginfo.get(), fd, ds->filename, size);
}

void MainServer::DeleteRecordedFiles(const DeleteStruct *ds)
{
    QString logInfo = QString("chanid %1 at %2")
        .arg(ds->chanid).arg(ds->recstartts.toString());

    MSqlQuery update(MSqlQuery::InitCon());
    MSqlQuery query(MSqlQuery::InitCon());
    query.prepare("SELECT basename, hostname, storagegroup FROM recordedfile "
                  "WHERE chanid = :CHANID AND starttime = :STARTTIME;");
    query.bindValue(":CHANID", ds->chanid);
    query.bindValue(":STARTTIME", ds->recstartts);

    if (!query.exec() || !query.isActive())
    {
        MythDB::DBError("RecordedFiles deletion", query);
        gContext->LogEntry("mythbackend", LP_ERROR, "Delete Recording Files",
                           QString("Error querying recordedfiles for %1.")
                                   .arg(logInfo));
    }

    QString basename;
    QString hostname;
    QString storagegroup;
    bool deleteInDB;
    while (query.next()) {
        basename = query.value(0).toString();
        hostname = query.value(1).toString();
        storagegroup = query.value(2).toString();
        deleteInDB = false;

        if (basename == ds->filename)
            deleteInDB = true;
        else
        {
            VERBOSE(VB_FILE, QString("DeleteRecordedFiles(%1), deleting '%2'")
                    .arg(logInfo).arg(query.value(0).toString()));

            StorageGroup sgroup(storagegroup);
            QString localFile = sgroup.FindRecordingFile(basename);
            QString url = QString("myth://%1@%2:%3/%4").arg(storagegroup)
                .arg(gContext->GetSettingOnHost("BackendServerIP", hostname))
                .arg(gContext->GetSettingOnHost("BackendServerPort", hostname))
                .arg(basename);

            if ((((hostname == gContext->GetHostName()) ||
                  (!localFile.isEmpty())) &&
                 (HandleDeleteFile(basename, storagegroup))) ||
                (((hostname != gContext->GetHostName()) ||
                  (localFile.isEmpty())) &&
                 (RemoteFile::DeleteFile(url))))
            {
                deleteInDB = true;
            }
        }

        if (deleteInDB)
        {
            update.prepare("DELETE FROM recordedfile "
                           "WHERE chanid = :CHANID "
                               "AND starttime = :STARTTIME "
                               "AND basename = :BASENAME ;");
            update.bindValue(":CHANID", ds->chanid);
            update.bindValue(":STARTTIME", ds->recstartts);
            update.bindValue(":BASENAME", basename);
            if (!update.exec())
            {
                MythDB::DBError("RecordedFiles deletion", update);
                gContext->LogEntry("mythbackend", LP_ERROR,
                       "Delete Recording Files",
                       QString("Error querying recordedfile (%1) for %2.")
                               .arg(query.value(1).toString())
                               .arg(logInfo));
            }
        }
    }
}

void MainServer::DoDeleteInDB(const DeleteStruct *ds)
{
    QString logInfo = QString("chanid %1 at %2")
        .arg(ds->chanid).arg(ds->recstartts.toString());

    MSqlQuery query(MSqlQuery::InitCon());
    query.prepare("DELETE FROM recorded WHERE chanid = :CHANID AND "
                  "title = :TITLE AND starttime = :STARTTIME;");
    query.bindValue(":CHANID", ds->chanid);
    query.bindValue(":TITLE", ds->title);
    query.bindValue(":STARTTIME", ds->recstartts);

    if (!query.exec() || !query.isActive())
    {
        MythDB::DBError("Recorded program deletion", query);
        gContext->LogEntry("mythbackend", LP_ERROR, "Delete Recording",
                           QString("Error deleting recorded table for %1.")
                                   .arg(logInfo));
    }

    sleep(1);

    // Notify the frontend so it can requery for Free Space
    QString msg = QString("RECORDING_LIST_CHANGE DELETE %1 %2")
        .arg(ds->chanid).arg(ds->recstartts.toString(Qt::ISODate));
    RemoteSendEvent(MythEvent(msg));

    // sleep a little to let frontends reload the recordings list
    sleep(3);

    query.prepare("DELETE FROM recordedmarkup "
                  "WHERE chanid = :CHANID AND starttime = :STARTTIME;");
    query.bindValue(":CHANID", ds->chanid);
    query.bindValue(":STARTTIME", ds->recstartts);

    if (!query.exec())
    {
        MythDB::DBError("Recorded program delete recordedmarkup", query);
        gContext->LogEntry("mythbackend", LP_ERROR, "Delete Recording",
                           QString("Error deleting recordedmarkup for %1.")
                                   .arg(logInfo));
    }

    query.prepare("DELETE FROM recordedseek "
                  "WHERE chanid = :CHANID AND starttime = :STARTTIME;");
    query.bindValue(":CHANID", ds->chanid);
    query.bindValue(":STARTTIME", ds->recstartts);

    if (!query.exec())
    {
        MythDB::DBError("Recorded program delete recordedseek", query);
        gContext->LogEntry("mythbackend", LP_ERROR, "Delete Recording",
                           QString("Error deleting recordedseek for %1.")
                                   .arg(logInfo));
    }
}

/**
 *  \brief Deletes links and unlinks the main file and returns the descriptor.
 *
 *  This is meant to be used with TruncateAndClose() to slowly shrink a
 *  large file and then eventually delete the file by closing the file
 *  descriptor.
 *
 *  \return fd for success, -1 for error, -2 for only a symlink deleted.
 */
int MainServer::DeleteFile(const QString &filename, bool followLinks,
                           bool deleteBrokenSymlinks)
{
    QFileInfo finfo(filename);
    int fd = -1, err = 0;
    QString linktext = "";
    QByteArray fname = filename.toLocal8Bit();

    VERBOSE(VB_FILE, QString("About to unlink/delete file: '%1'")
            .arg(fname.constData()));

    QString errmsg = QString("Delete Error '%1'").arg(fname.constData());
    if (finfo.isSymLink())
    {
        linktext = getSymlinkTarget(filename);
        QByteArray alink = linktext.toLocal8Bit();
        errmsg += QString(" -> '%2'").arg(alink.constData());
    }

    if (followLinks && finfo.isSymLink())
    {
        if (!finfo.exists() && deleteBrokenSymlinks)
            err = unlink(fname.constData());
        else
        {
            fd = OpenAndUnlink(linktext);
            if (fd >= 0)
                err = unlink(fname.constData());
        }
    }
    else if (!finfo.isSymLink())
    {
        fd = OpenAndUnlink(filename);
    }
    else // just delete symlinks immediately
    {
        err = unlink(fname.constData());
        if (err == 0)
            return -2; // valid result, not an error condition
    }

    if (fd < 0)
        VERBOSE(VB_IMPORTANT, errmsg + ENO);

    return fd;
}

/** \fn MainServer::OpenAndUnlink(const QString&)
 *  \brief Opens a file, unlinks it and returns the file descriptor.
 *
 *  This is used by DeleteFile(const QString&,bool) to delete recordings.
 *  In order to actually delete the file from the filesystem the user of
 *  this function must close the return file descriptor.
 *
 *  \return fd for success, negative number for error.
 */
int MainServer::OpenAndUnlink(const QString &filename)
{
    QByteArray fname = filename.toLocal8Bit();
    QString msg = QString("Error deleting '%1'").arg(fname.constData());
    int fd = open(fname.constData(), O_WRONLY);

    if (fd == -1)
    {
        VERBOSE(VB_IMPORTANT, msg + " could not open " + ENO);
        return -1;
    }

    if (unlink(fname.constData()))
    {
        VERBOSE(VB_IMPORTANT, msg + " could not unlink " + ENO);
        close(fd);
        return -1;
    }

    return fd;
}

/**
 *  \brief Repeatedly truncate an open file in small increments.
 *
 *   When the file is small enough this closes the file and returns.
 *
 *   NOTE: This acquires a lock so that only one instance of TruncateAndClose()
 *         is running at a time.
 */
bool MainServer::TruncateAndClose(ProgramInfo *pginfo, int fd,
                                  const QString &filename, off_t fsize)
{
    QMutexLocker locker(&truncate_and_close_lock);

    if (pginfo)
    {
        pginfo->pathname = filename;
        pginfo->MarkAsInUse(true, kTruncatingDeleteInUseID);
    }

    int cards = 5;

    MSqlQuery query(MSqlQuery::InitCon());
    query.prepare("SELECT COUNT(cardid) FROM capturecard;");
    if (query.exec() && query.isActive() && query.size() && query.next())
        cards = query.value(0).toInt();

    // Time between truncation steps in milliseconds
    const size_t sleep_time = 500;
    const size_t min_tps    = 8 * 1024 * 1024;
    const size_t calc_tps   = (size_t) (cards * 1.2 * (22200000LL / 8));
    const size_t tps = max(min_tps, calc_tps);
    const size_t increment  = (size_t) (tps * (sleep_time * 0.001f));

    VERBOSE(VB_FILE,
            QString("Truncating '%1' by %2 MB every %3 milliseconds")
            .arg(filename)
            .arg(increment / (1024.0 * 1024.0), 0, 'f', 2)
            .arg(sleep_time));

    int count = 0;
    while (fsize > 0)
    {
        //VERBOSE(VB_FILE, QString("Truncating '%1' to %2 MB")
        //        .arg(filename).arg(fsize / (1024.0 * 1024.0), 0, 'f', 2));

        int err = ftruncate(fd, fsize);
        if (err)
        {
            VERBOSE(VB_IMPORTANT, QString("Error truncating '%1'")
                    .arg(filename) + ENO);
            if (pginfo)
                pginfo->MarkAsInUse(false, kTruncatingDeleteInUseID);
            return 0 == close(fd);
        }

        fsize -= increment;

        if (pginfo && ((count % 100) == 0))
            pginfo->UpdateInUseMark(true);

        count++;

        usleep(sleep_time * 1000);
    }

    bool ok = (0 == close(fd));

    if (pginfo)
        pginfo->MarkAsInUse(false, kTruncatingDeleteInUseID);

    VERBOSE(VB_FILE, QString("Finished truncating '%1'").arg(filename));

    return ok;
}

void MainServer::HandleCheckRecordingActive(QStringList &slist,
                                            PlaybackSock *pbs)
{
    MythSocket *pbssock = NULL;
    if (pbs)
        pbssock = pbs->getSocket();

    ProgramInfo *pginfo = new ProgramInfo();
    pginfo->FromStringList(slist, 1);

    int result = 0;

    if (ismaster && pginfo->hostname != gContext->GetHostName())
    {
        PlaybackSock *slave = getSlaveByHostname(pginfo->hostname);
        if (slave)
        {
            result = slave->CheckRecordingActive(pginfo);
            slave->DownRef();
        }
    }
    else
    {
        QMap<int, EncoderLink *>::Iterator iter = encoderList->begin();
        for (; iter != encoderList->end(); ++iter)
        {
            EncoderLink *elink = *iter;

            if (elink->IsLocal() && elink->MatchesRecording(pginfo))
                result = iter.key();
        }
    }

    QStringList outputlist( QString::number(result) );
    if (pbssock)
        SendResponse(pbssock, outputlist);

    delete pginfo;
    return;
}

void MainServer::HandleStopRecording(QStringList &slist, PlaybackSock *pbs)
{
    RecordingInfo recinfo;
    if (recinfo.FromStringList(slist, 1))
        DoHandleStopRecording(recinfo, pbs);
}

void MainServer::DoHandleStopRecording(
    RecordingInfo &recinfo, PlaybackSock *pbs)
{
    MythSocket *pbssock = NULL;
    if (pbs)
        pbssock = pbs->getSocket();

    if (ismaster && recinfo.hostname != gContext->GetHostName())
    {
        PlaybackSock *slave = getSlaveByHostname(recinfo.hostname);

        int num = -1;

        if (slave)
        {
            num = slave->StopRecording(&recinfo);

            if (num > 0)
            {
                (*encoderList)[num]->StopRecording();
                recinfo.recstatus = rsRecorded;
                if (m_sched)
                    m_sched->UpdateRecStatus(&recinfo);
            }
            if (pbssock)
            {
                QStringList outputlist( "0" );
                SendResponse(pbssock, outputlist);
            }

            slave->DownRef();
            return;
        }
        else
        {
            // If the slave is unreachable, we can assume that the
            // recording has stopped and the status should be updated.
            // Continue so that the master can try to update the endtime
            // of the file is in a shared directory.
            recinfo.recstatus = rsRecorded;
            if (m_sched)
                m_sched->UpdateRecStatus(&recinfo);
        }

    }

    int recnum = -1;

    QMap<int, EncoderLink *>::Iterator iter = encoderList->begin();
    for (; iter != encoderList->end(); ++iter)
    {
        EncoderLink *elink = *iter;

        if (elink->IsLocal() && elink->MatchesRecording(&recinfo))
        {
            recnum = iter.key();

            elink->StopRecording();

            while (elink->IsBusyRecording() ||
                   elink->GetState() == kState_ChangingState)
            {
                usleep(100);
            }

            if (ismaster)
            {
                recinfo.recstatus = rsRecorded;
                if (m_sched)
                    m_sched->UpdateRecStatus(&recinfo);
            }
        }
    }

    if (pbssock)
    {
        QStringList outputlist( QString::number(recnum) );
        SendResponse(pbssock, outputlist);
    }
}

void MainServer::HandleDeleteRecording(QString &chanid, QString &starttime,
                                       PlaybackSock *pbs,
                                       bool forceMetadataDelete)
{
    ProgramInfo *pginfo =
        ProgramInfo::GetProgramFromRecorded(chanid, starttime);

    if (!pginfo)
    {
        MythSocket *pbssock = NULL;
        if (pbs)
            pbssock = pbs->getSocket();

        QStringList outputlist( QString::number(0) );

        SendResponse(pbssock, outputlist);
        return;
    }

    RecordingInfo ri(*pginfo);
    delete pginfo;

    DoHandleDeleteRecording(ri, pbs, forceMetadataDelete);
}

void MainServer::HandleDeleteRecording(QStringList &slist, PlaybackSock *pbs,
                                       bool forceMetadataDelete)
{
    RecordingInfo recinfo;
    if (recinfo.FromStringList(slist, 1))
        DoHandleDeleteRecording(recinfo, pbs, forceMetadataDelete);
}

void MainServer::DoHandleDeleteRecording(
    RecordingInfo &recinfo, PlaybackSock *pbs,
    bool forceMetadataDelete, bool expirer)
{
    int resultCode = -1;
    MythSocket *pbssock = NULL;
    if (pbs)
        pbssock = pbs->getSocket();

    bool justexpire = expirer ? false :
            (gContext->GetNumSetting("AutoExpireInsteadOfDelete") &&
            (recinfo.recgroup != "Deleted") && (recinfo.recgroup != "LiveTV"));

    QString filename = GetPlaybackURL(&recinfo, false);
    if (filename.isEmpty())
    {
        VERBOSE(VB_IMPORTANT,
                QString("ERROR when trying to delete file for %1 @ %2.  Unable "
                        "to determine filename of recording.")
                        .arg(recinfo.chanid)
                        .arg(recinfo.recstartts.toString(Qt::ISODate)));

        if (pbssock)
        {
            resultCode = -2;
            QStringList outputlist(QString::number(resultCode));
            SendResponse(pbssock, outputlist);
        }

        return;
    }

    if (justexpire && !forceMetadataDelete && recinfo.filesize > (1024 * 1024) )
    {
        recinfo.ApplyRecordRecGroupChange("Deleted");
        recinfo.SetAutoExpire(kDeletedAutoExpire);
        recinfo.UpdateLastDelete(true);
        if (recinfo.recstatus == rsRecording)
            DoHandleStopRecording(recinfo, NULL);
        QStringList outputlist( QString::number(0) );
        SendResponse(pbssock, outputlist);
        return;
    }

    // If this recording was made by a another recorder, and that
    // recorder is available, tell it to do the deletion.
    if (ismaster && recinfo.hostname != gContext->GetHostName())
    {
        PlaybackSock *slave = getSlaveByHostname(recinfo.hostname);

        int num = -1;

        if (slave)
        {
            num = slave->DeleteRecording(&recinfo, forceMetadataDelete);

            if (num > 0)
            {
                (*encoderList)[num]->StopRecording();
                recinfo.recstatus = rsRecorded;
                if (m_sched)
                    m_sched->UpdateRecStatus(&recinfo);
            }

            if (pbssock)
            {
                QStringList outputlist( QString::number(num) );
                SendResponse(pbssock, outputlist);
            }

            slave->DownRef();
            return;
        }
    }

    // Tell all encoders to stop recording to the file being deleted.
    // Hopefully this is never triggered.

    QMap<int, EncoderLink *>::Iterator iter = encoderList->begin();
    for (; iter != encoderList->end(); ++iter)
    {
        EncoderLink *elink = *iter;

        if (elink->IsLocal() && elink->MatchesRecording(&recinfo))
        {
            resultCode = iter.key();

            elink->StopRecording(true);

            while (elink->IsBusyRecording() ||
                   elink->GetState() == kState_ChangingState)
            {
                usleep(100);
            }

            if (ismaster)
            {
                recinfo.recstatus = rsRecorded;
                if (m_sched)
                    m_sched->UpdateRecStatus(&recinfo);
            }
        }
    }

    QFile checkFile(filename);
    bool fileExists = checkFile.exists();
    if (!fileExists)
    {
        QFile checkFileUTF8(QString::fromUtf8(filename.toAscii().constData()));
        fileExists = checkFileUTF8.exists();
        if (fileExists)
            filename = QString::fromUtf8(filename.toAscii().constData());
    }

    // Allow deleting of files where the recording failed meaning size == 0
    // But do not allow deleting of files that appear to be completely absent.
    // The latter condition indicates the filesystem containing the file is
    // most likely absent and deleting the file metadata is unsafe.
    if ((fileExists) || (recinfo.filesize == 0) || (forceMetadataDelete))
    {
        DeleteStruct *ds = new DeleteStruct;
        ds->ms = this;
        ds->filename = filename;
        ds->title = recinfo.title;
        ds->chanid = recinfo.chanid;
        ds->recstartts = recinfo.recstartts;
        ds->recendts = recinfo.recendts;
        ds->forceMetadataDelete = forceMetadataDelete;

        recinfo.SetDeleteFlag(true);

        pthread_t deleteThread;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&deleteThread, &attr, SpawnDeleteThread, ds);
        pthread_attr_destroy(&attr);
    }
    else
    {
        QString logInfo = QString("chanid %1 at %2")
                              .arg(recinfo.chanid)
                              .arg(recinfo.recstartts.toString());
        VERBOSE(VB_IMPORTANT,
                QString("ERROR when trying to delete file: %1. File doesn't "
                        "exist.  Database metadata will not be removed.")
                        .arg(filename));
        gContext->LogEntry("mythbackend", LP_WARNING, "Delete Recording",
                           QString("File %1 does not exist for %2 when trying "
                                   "to delete recording.")
                                   .arg(filename).arg(logInfo));
        resultCode = -2;
    }

    if (pbssock)
    {
        QStringList outputlist( QString::number(resultCode) );
        SendResponse(pbssock, outputlist);
    }

    // Tell MythTV frontends that the recording list needs to be updated.
    if ((fileExists) || (recinfo.filesize == 0) || (forceMetadataDelete))
    {
        SendMythSystemEvent(QString("REC_DELETED CHANID %1 STARTTIME %2")
                            .arg(recinfo.chanid)
                            .arg(recinfo.recstartts.toString(Qt::ISODate)));

        recinfo.SendDeletedEvent();
    }
}

void MainServer::HandleUndeleteRecording(QStringList &slist, PlaybackSock *pbs)
{
    bool ok = false;
    RecordingInfo recinfo;
    if (slist.size() == 3)
    {
        ok = recinfo.LoadProgramFromRecorded(
            slist[1].toUInt(), QDateTime::fromString(slist[2], Qt::ISODate));
    }
    else
    {
        ok = recinfo.FromStringList(slist, 1);
    }
    if (ok)
        DoHandleUndeleteRecording(recinfo, pbs);
}

void MainServer::DoHandleUndeleteRecording(
    RecordingInfo &recinfo, PlaybackSock *pbs)
{
    bool ret = -1;
    bool undelete_possible =
            gContext->GetNumSetting("AutoExpireInsteadOfDelete", 0);
    MythSocket *pbssock = NULL;
    if (pbs)
        pbssock = pbs->getSocket();

    if (undelete_possible)
    {
        recinfo.ApplyRecordRecGroupChange("Default");
        recinfo.UpdateLastDelete(false);
        recinfo.SetAutoExpire(kDisableAutoExpire);
        ret = 0;
    }

    QStringList outputlist( QString::number(ret) );
    SendResponse(pbssock, outputlist);
}

void MainServer::HandleRescheduleRecordings(int recordid, PlaybackSock *pbs)
{
    QStringList result;
    if (m_sched)
    {
        m_sched->Reschedule(recordid);
        result = QStringList( QString::number(1) );
    }
    else
        result = QStringList( QString::number(0) );

    if (pbs)
    {
        MythSocket *pbssock = pbs->getSocket();
        if (pbssock)
            SendResponse(pbssock, result);
    }
}

void MainServer::HandleForgetRecording(QStringList &slist, PlaybackSock *pbs)
{
    RecordingInfo pginfo;
    pginfo.FromStringList(slist, 1);

    MythSocket *pbssock = NULL;
    if (pbs)
        pbssock = pbs->getSocket();

    pginfo.ForgetHistory();

    if (pbssock)
    {
        QStringList outputlist( QString::number(0) );
        SendResponse(pbssock, outputlist);
    }
}

/*
 * \addtogroup myth_network_protocol
 * \par        GO_TO_SLEEP
 * Commands a slave to go to sleep
 */
void MainServer::HandleGoToSleep(PlaybackSock *pbs)
{
    QStringList strlist;

    QString sleepCmd = gContext->GetSetting("SleepCommand");
    if (!sleepCmd.isEmpty())
    {
        strlist << "OK";
        SendResponse(pbs->getSocket(), strlist);
        VERBOSE(VB_IMPORTANT, "Received GO_TO_SLEEP command from master, "
                "running SleepCommand.");
        myth_system(sleepCmd);
    }
    else
    {
        strlist << "ERROR: SleepCommand is empty";
        VERBOSE(VB_IMPORTANT,
                "ERROR: in HandleGoToSleep(), but no SleepCommand found!");
        SendResponse(pbs->getSocket(), strlist);
    }
}

/**
 * \addtogroup myth_network_protocol
 * \par        QUERY_FREE_SPACE
 * Returns the free space on this backend, as a list of hostname, directory,
 * 1, -1, total size, used (both in K and 64bit, so two 32bit numbers each).
 * \par        QUERY_FREE_SPACE_LIST
 * Returns the free space on \e all hosts. (each host as above,
 * except that the directory becomes a URL, and a TotalDiskSpace is appended)
 */
void MainServer::HandleQueryFreeSpace(PlaybackSock *pbs, bool allHosts)
{
    QStringList strlist;

    BackendQueryDiskSpace(strlist, encoderList, allHosts, allHosts);

    SendResponse(pbs->getSocket(), strlist);
}

/**
 * \addtogroup myth_network_protocol
 * \par        QUERY_FREE_SPACE_SUMMARY
 * Summarises the free space on this backend, as list of total size, used
 */
void MainServer::HandleQueryFreeSpaceSummary(PlaybackSock *pbs)
{
    QStringList fullStrList;
    QStringList strList;

    BackendQueryDiskSpace(fullStrList, encoderList, true, true);

    // The TotalKB and UsedKB are the last two numbers encoded in the list
    unsigned int index = fullStrList.size() - 4;
    strList << fullStrList[index++];
    strList << fullStrList[index++];
    strList << fullStrList[index++];
    strList << fullStrList[index++];

    SendResponse(pbs->getSocket(), strList);
}

/**
 * \addtogroup myth_network_protocol
 * \par        QUERY_LOAD
 * Returns the Unix load on this backend
 * (three floats - the average over 1, 5 and 15 mins).
 */
void MainServer::HandleQueryLoad(PlaybackSock *pbs)
{
    MythSocket *pbssock = pbs->getSocket();

    QStringList strlist;

    double loads[3];
    if (getloadavg(loads,3) == -1)
    {
        strlist << "ERROR";
        strlist << "getloadavg() failed";
    }
    else
        strlist << QString::number(loads[0])
                << QString::number(loads[1])
                << QString::number(loads[2]);

    SendResponse(pbssock, strlist);
}

/**
 * \addtogroup myth_network_protocol
 * \par        QUERY_UPTIME
 * Returns the number of seconds this backend's host has been running
 */
void MainServer::HandleQueryUptime(PlaybackSock *pbs)
{
    MythSocket    *pbssock = pbs->getSocket();
    QStringList strlist;
    time_t      uptime;

    if (getUptime(uptime))
        strlist << QString::number(uptime);
    else
    {
        strlist << "ERROR";
        strlist << "Could not determine uptime.";
    }

    SendResponse(pbssock, strlist);
}

/**
 * \addtogroup myth_network_protocol
 * \par        QUERY_HOSTNAME
 * Returns the hostname of this backend
 */
void MainServer::HandleQueryHostname(PlaybackSock *pbs)
{
    MythSocket    *pbssock = pbs->getSocket();
    QStringList strlist;

    strlist << gContext->GetHostName();

    SendResponse(pbssock, strlist);
}

/**
 * \addtogroup myth_network_protocol
 * \par        QUERY_MEMSTATS
 * Returns total RAM, free RAM, total VM and free VM (all in MB)
 */
void MainServer::HandleQueryMemStats(PlaybackSock *pbs)
{
    MythSocket    *pbssock = pbs->getSocket();
    QStringList strlist;
    int         totalMB, freeMB, totalVM, freeVM;

    if (getMemStats(totalMB, freeMB, totalVM, freeVM))
        strlist << QString::number(totalMB) << QString::number(freeMB)
                << QString::number(totalVM) << QString::number(freeVM);
    else
    {
        strlist << "ERROR";
        strlist << "Could not determine memory stats.";
    }

    SendResponse(pbssock, strlist);
}

/**
 * \addtogroup myth_network_protocol
 * \par        QUERY_TIME_ZONE
 * Returns time zone ID, current offset, current time
 */
void MainServer::HandleQueryTimeZone(PlaybackSock *pbs)
{
    MythSocket *pbssock = pbs->getSocket();
    QStringList strlist;
    strlist << getTimeZoneID() << QString::number(calc_utc_offset())
            << mythCurrentDateTime().toString(Qt::ISODate);

    SendResponse(pbssock, strlist);
}

/**
 * \addtogroup myth_network_protocol
 * \par        QUERY_CHECKFILE \e checkslaves \e programinfo
 */
void MainServer::HandleQueryCheckFile(QStringList &slist, PlaybackSock *pbs)
{
    MythSocket *pbssock = pbs->getSocket();
    bool checkSlaves = slist[1].toInt();

    RecordingInfo *pginfo = new RecordingInfo();
    pginfo->FromStringList(slist, 2);

    int exists = 0;

    if ((ismaster) &&
        (pginfo->hostname != gContext->GetHostName()) &&
        (checkSlaves))
    {
        PlaybackSock *slave = getSlaveByHostname(pginfo->hostname);

        if (slave)
        {
             exists = slave->CheckFile(pginfo);
             slave->DownRef();

             QStringList outputlist( QString::number(exists) );
             if (exists)
                 outputlist << pginfo->pathname;
             else
                 outputlist << "";

             SendResponse(pbssock, outputlist);
             delete pginfo;
             return;
        }
    }

    QString pburl = GetPlaybackURL(pginfo);
    QFile checkFile(pburl);

    if (checkFile.exists() == true)
        exists = 1;

    QStringList strlist( QString::number(exists) );
    if (exists)
        strlist << pburl;
    else
        strlist << "";
    SendResponse(pbssock, strlist);

    delete pginfo;
}


/**
 * \addtogroup myth_network_protocol
 * \par        QUERY_FILE_HASH \e storagegroup \e filename
 */
void MainServer::HandleQueryFileHash(QStringList &slist, PlaybackSock *pbs)
{
    QString filename = slist[1];
    QString storageGroup = "Default";
    QStringList retlist;

    if (slist.size() > 2)
        storageGroup = slist[2];

    if ((filename.isEmpty()) ||
        (filename.contains("/../")) ||
        (filename.startsWith("../")))
    {
        VERBOSE(VB_IMPORTANT, QString("ERROR checking for file, filename '%1' "
                "fails sanity checks").arg(filename));
        retlist << "";
        SendResponse(pbs->getSocket(), retlist);
        return;
    }

    if (storageGroup.isEmpty())
        storageGroup = "Default";

    StorageGroup sgroup(storageGroup, gContext->GetHostName());

    QString fullname = sgroup.FindRecordingFile(filename);
    QString hash = FileHash(fullname);

    retlist << hash;

    SendResponse(pbs->getSocket(), retlist);
}

/**
 * \addtogroup myth_network_protocol
 * \par        QUERY_FILE_EXISTS \e storagegroup \e filename
 */
void MainServer::HandleQueryFileExists(QStringList &slist, PlaybackSock *pbs)
{
    QString filename = slist[1];
    QString storageGroup = "Default";
    QStringList retlist;

    if (slist.size() > 2)
        storageGroup = slist[2];

    if ((filename.isEmpty()) ||
        (filename.contains("/../")) ||
        (filename.startsWith("../")))
    {
        VERBOSE(VB_IMPORTANT, QString("ERROR checking for file, filename '%1' "
                "fails sanity checks").arg(filename));
        retlist << "0";
        SendResponse(pbs->getSocket(), retlist);
        return;
    }

    if (storageGroup.isEmpty())
        storageGroup = "Default";

    StorageGroup sgroup(storageGroup, gContext->GetHostName());

    QString fullname = sgroup.FindRecordingFile(filename);

    if (!fullname.isEmpty())
    {
        retlist << "1";
        retlist << fullname;
    }
    else
        retlist << "0";

    SendResponse(pbs->getSocket(), retlist);
}

void MainServer::getGuideDataThrough(QDateTime &GuideDataThrough)
{
    MSqlQuery query(MSqlQuery::InitCon());
    query.prepare("SELECT MAX(endtime) FROM program WHERE manualid = 0;");

    if (query.exec() && query.isActive() && query.size())
    {
        query.next();
        if (query.isValid())
            GuideDataThrough = QDateTime::fromString(query.value(0).toString(),
                                                     Qt::ISODate);
    }
}

void MainServer::HandleQueryGuideDataThrough(PlaybackSock *pbs)
{
    QDateTime GuideDataThrough;
    MythSocket *pbssock = pbs->getSocket();
    QStringList strlist;

    getGuideDataThrough(GuideDataThrough);

    if (GuideDataThrough.isNull())
        strlist << QString("0000-00-00 00:00");
    else
        strlist << QDateTime(GuideDataThrough).toString("yyyy-MM-dd hh:mm");

    SendResponse(pbssock, strlist);
}

void MainServer::HandleGetPendingRecordings(PlaybackSock *pbs,
                                            QString tmptable, int recordid)
{
    MythSocket *pbssock = pbs->getSocket();

    QStringList strList;

    if (m_sched)
    {
        if (tmptable.isEmpty())
            m_sched->getAllPending(strList);
        else
        {
            Scheduler *sched = new Scheduler(false, encoderList,
                                             tmptable, m_sched);
            sched->FillRecordListFromDB(recordid);
            sched->getAllPending(strList);
            delete sched;

            if (recordid > 0)
            {
                MSqlQuery query(MSqlQuery::InitCon());
                query.prepare("SELECT NULL FROM record "
                              "WHERE recordid = :RECID;");
                query.bindValue(":RECID", recordid);

                if (query.exec() && query.size())
                {
                    RecordingRule *record = new RecordingRule();
                    record->m_recordID = recordid;
                    if (record->Load() &&
                        record->m_searchType == kManualSearch)
                        HandleRescheduleRecordings(recordid, NULL);
                    delete record;
                }
                query.prepare("DELETE FROM program WHERE manualid = :RECID;");
                query.bindValue(":RECID", recordid);
                if (!query.exec())
                    MythDB::DBError("MainServer::HandleGetPendingRecordings "
                                    "- delete", query);
            }
        }
    }
    else
    {
        strList << QString::number(0);
        strList << QString::number(0);
    }

    SendResponse(pbssock, strList);
}

void MainServer::HandleGetScheduledRecordings(PlaybackSock *pbs)
{
    MythSocket *pbssock = pbs->getSocket();

    QStringList strList;

    if (m_sched)
        m_sched->getAllScheduled(strList);
    else
        strList << QString::number(0);

    SendResponse(pbssock, strList);
}

void MainServer::HandleGetConflictingRecordings(QStringList &slist,
                                                PlaybackSock *pbs)
{
    MythSocket *pbssock = pbs->getSocket();

    RecordingInfo pginfo;
    pginfo.FromStringList(slist, 1);

    QStringList strlist;

    if (m_sched)
        m_sched->getConflicting(&pginfo, strlist);
    else
        strlist << QString::number(0);

    SendResponse(pbssock, strlist);
}

void MainServer::HandleGetExpiringRecordings(PlaybackSock *pbs)
{
    MythSocket *pbssock = pbs->getSocket();

    QStringList strList;

    if (m_expirer)
        m_expirer->GetAllExpiring(strList);
    else
        strList << QString::number(0);

    SendResponse(pbssock, strList);
}

void MainServer::HandleSGGetFileList(QStringList &sList,
                                     PlaybackSock *pbs)
{
    MythSocket *pbssock = pbs->getSocket();

    QString host = gContext->GetHostName();
    QString wantHost = sList.at(1);
    QString groupname = sList.at(2);
    QString path = sList.at(3);
    bool fileNamesOnly = false;
    QStringList strList;

    if (sList.size() >= 5)
        fileNamesOnly = sList.at(4).toInt();

    bool slaveUnreachable = false;

    VERBOSE(VB_FILE, QString("HandleSGGetFileList: group = %1  host = %2  path = %3 wanthost = %4").arg(groupname).arg(host).arg(path).arg(wantHost));

    if ((host.toLower() == wantHost.toLower()) ||
        (gContext->GetSetting("BackendServerIP") == wantHost))
    {
        StorageGroup sg(groupname, host);
        VERBOSE(VB_FILE, QString("HandleSGGetFileList: Getting local info"));
        if (fileNamesOnly)
            strList = sg.GetFileList(path);
        else
            strList = sg.GetFileInfoList(path);
    }
    else
    {
        PlaybackSock *slave = getSlaveByHostname(wantHost);
        if (slave)
        {
            VERBOSE(VB_FILE, QString("HandleSGGetFileList: Getting remote info"));
            strList = slave->GetSGFileList(wantHost, groupname, path,
                                           fileNamesOnly);
            slave->DownRef();
            slaveUnreachable = false;
        }
        else
        {
            VERBOSE(VB_FILE, QString("HandleSGGetFileList: Failed to grab slave socket : %1 :").arg(wantHost));
            slaveUnreachable = true;
        }

    }

    if (slaveUnreachable)
        strList << "SLAVE UNREACHABLE: " << host;

    if (strList.count() == 0 || (strList.at(0) == "0"))
        strList << "EMPTY LIST";

    SendResponse(pbssock, strList);
}

void MainServer::HandleSGFileQuery(QStringList &sList,
                                     PlaybackSock *pbs)
{
    MythSocket *pbssock = pbs->getSocket();

    QString host = gContext->GetHostName();
    QString wantHost = sList.at(1);
    QString groupname = sList.at(2);
    QString filename = sList.at(3);
    QStringList strList;

    bool slaveUnreachable = false;

    VERBOSE(VB_FILE, QString("HandleSGFileQuery: group = %1  host = %2  filename = %3 wanthost = %4").arg(groupname).arg(host).arg(filename).arg(wantHost));

    if (host.toLower() == wantHost.toLower())
    {
        StorageGroup sg(groupname, host);
        VERBOSE(VB_FILE, QString("HandleSGFileQuery: Getting local info"));
        strList = sg.GetFileInfo(filename);
    }
    else
    {
        PlaybackSock *slave = getSlaveByHostname(wantHost);
        if (slave)
        {
            VERBOSE(VB_FILE, QString("HandleSGFileQuery: Getting remote info"));
            strList = slave->GetSGFileQuery(wantHost, groupname, filename);
            slave->DownRef();
            slaveUnreachable = false;
        }
        else
        {
            VERBOSE(VB_FILE, QString("HandleSGFileQuery: Failed to grab slave socket : %1 :").arg(wantHost));
            slaveUnreachable = true;
        }

    }

    if (slaveUnreachable)
        strList << "SLAVE UNREACHABLE: " << host;

    if (strList.count() == 0 || (strList.at(0) == "0"))
        strList << "EMPTY LIST";

    SendResponse(pbssock, strList);
}

void MainServer::HandleLockTuner(PlaybackSock *pbs, int cardid)
{
    MythSocket *pbssock = pbs->getSocket();
    QString pbshost = pbs->getHostname();

    QStringList strlist;
    int retval;

    EncoderLink *encoder = NULL;
    QString enchost;

    QMap<int, EncoderLink *>::Iterator iter = encoderList->begin();
    for (; iter != encoderList->end(); ++iter)
    {
        EncoderLink *elink = *iter;

        // we're looking for a specific card but this isn't the one we want
        if ((cardid != -1) && (cardid != elink->GetCardID()))
            continue;

        if (elink->IsLocal())
            enchost = gContext->GetHostName();
        else
            enchost = elink->GetHostName();

        if ((enchost == pbshost) &&
            (elink->IsConnected()) &&
            (!elink->IsBusy()) &&
            (!elink->IsTunerLocked()))
        {
            encoder = elink;
            break;
        }
    }

    if (encoder)
    {
        retval = encoder->LockTuner();

        if (retval != -1)
        {
            QString msg = QString("Cardid %1 LOCKed for external use on %2.")
                                  .arg(retval).arg(pbshost);
            VERBOSE(VB_GENERAL, msg);

            MSqlQuery query(MSqlQuery::InitCon());
            query.prepare("SELECT videodevice, audiodevice, "
                          "vbidevice "
                          "FROM capturecard "
                          "WHERE cardid = :CARDID ;");
            query.bindValue(":CARDID", retval);

            if (query.exec() && query.isActive() && query.size())
            {
                // Success
                query.next();
                strlist << QString::number(retval)
                        << query.value(0).toString()
                        << query.value(1).toString()
                        << query.value(2).toString();

                if (m_sched)
                    m_sched->Reschedule(0);

                SendResponse(pbssock, strlist);
                return;
            }
            else
                VERBOSE(VB_IMPORTANT, "MainServer::LockTuner(): Could not find "
                        "card info in database");
        }
        else
        {
            // Tuner already locked
            strlist << "-2" << "" << "" << "";
            SendResponse(pbssock, strlist);
            return;
        }
    }

    strlist << "-1" << "" << "" << "";
    SendResponse(pbssock, strlist);
}

void MainServer::HandleFreeTuner(int cardid, PlaybackSock *pbs)
{
    MythSocket *pbssock = pbs->getSocket();
    QStringList strlist;
    EncoderLink *encoder = NULL;

    QMap<int, EncoderLink *>::Iterator iter = encoderList->find(cardid);
    if (iter == encoderList->end())
    {
        VERBOSE(VB_IMPORTANT, "MainServer::HandleFreeTuner() " +
                QString("Unknown encoder: %1").arg(cardid));
        strlist << "FAILED";
    }
    else
    {
        encoder = *iter;
        encoder->FreeTuner();

        QString msg = QString("Cardid %1 FREED from external use on %2.")
                              .arg(cardid).arg(pbs->getHostname());
        VERBOSE(VB_GENERAL, msg);

        if (m_sched)
            m_sched->Reschedule(0);

        strlist << "OK";
    }

    SendResponse(pbssock, strlist);
}

void MainServer::HandleGetFreeRecorder(PlaybackSock *pbs)
{
    MythSocket *pbssock = pbs->getSocket();
    QString pbshost = pbs->getHostname();

    vector<uint> excluded_cardids;
    QStringList strlist;
    int retval = -1;

    EncoderLink *encoder = NULL;
    QString enchost;

    bool lastcard = false;

    if (gContext->GetSetting("LastFreeCard", "0") == "1")
        lastcard = true;

    QMap<int, EncoderLink *>::Iterator iter = encoderList->begin();
    for (; iter != encoderList->end(); ++iter)
    {
        EncoderLink *elink = *iter;

        if (!lastcard)
        {
            if (elink->IsLocal())
                enchost = gContext->GetHostName();
            else
                enchost = elink->GetHostName();

            if (enchost == pbshost && elink->IsConnected() &&
                !elink->IsTunerLocked() &&
                !elink->GetFreeInputs(excluded_cardids).empty())
            {
                encoder = elink;
                retval = iter.key();
                VERBOSE(VB_RECORD, QString("Card %1 is local.")
                        .arg(iter.key()));
                break;
            }
        }

        if ((retval == -1 || lastcard) && elink->IsConnected() &&
            !elink->IsTunerLocked() &&
            !elink->GetFreeInputs(excluded_cardids).empty())
        {
            encoder = elink;
            retval = iter.key();
        }
        VERBOSE(VB_RECORD, QString("Checking card %1. Best card so far %2")
                .arg(iter.key()).arg(retval));
    }

    strlist << QString::number(retval);

    if (encoder)
    {
        if (encoder->IsLocal())
        {
            strlist << gContext->GetSetting("BackendServerIP");
            strlist << gContext->GetSetting("BackendServerPort");
        }
        else
        {
            strlist << gContext->GetSettingOnHost("BackendServerIP",
                                                  encoder->GetHostName(),
                                                  "nohostname");
            strlist << gContext->GetSettingOnHost("BackendServerPort",
                                                  encoder->GetHostName(), "-1");
        }
    }
    else
    {
        strlist << "nohost";
        strlist << "-1";
    }

    SendResponse(pbssock, strlist);
}

void MainServer::HandleGetFreeRecorderCount(PlaybackSock *pbs)
{
    MythSocket *pbssock = pbs->getSocket();

    vector<uint> excluded_cardids;
    QStringList strlist;
    int count = 0;

    QMap<int, EncoderLink *>::Iterator iter = encoderList->begin();
    for (; iter != encoderList->end(); ++iter)
    {
        EncoderLink *elink = *iter;

        if (elink->IsConnected() && !elink->IsTunerLocked() &&
            !elink->GetFreeInputs(excluded_cardids).empty())
        {
            count++;
        }
    }

    strlist << QString::number(count);

    SendResponse(pbssock, strlist);
}

void MainServer::HandleGetFreeRecorderList(PlaybackSock *pbs)
{
    MythSocket *pbssock = pbs->getSocket();

    QStringList strlist;

    QMap<int, EncoderLink *>::Iterator iter = encoderList->begin();
    for (; iter != encoderList->end(); ++iter)
    {
        EncoderLink *elink = *iter;

        if (elink->IsConnected() && !elink->IsTunerLocked() &&
            !elink->IsBusy())
        {
            strlist << QString::number(iter.key());
        }
    }

    if (strlist.size() == 0)
        strlist << "0";

    SendResponse(pbssock, strlist);
}

void MainServer::HandleGetNextFreeRecorder(QStringList &slist,
                                           PlaybackSock *pbs)
{
    MythSocket *pbssock = pbs->getSocket();
    QString pbshost = pbs->getHostname();

    QStringList strlist;
    int retval = -1;
    int currrec = slist[1].toInt();

    EncoderLink *encoder = NULL;
    QString enchost;

    VERBOSE(VB_RECORD, QString("Getting next free recorder after : %1")
            .arg(currrec));

    // find current recorder
    QMap<int, EncoderLink *>::Iterator iter, curr = encoderList->find(currrec);

    if (currrec > 0 && curr != encoderList->end())
    {
        vector<uint> excluded_cardids;
        excluded_cardids.push_back(currrec);

        // cycle through all recorders
        for (iter = curr;;)
        {
            EncoderLink *elink;

            // last item? go back
            if (++iter == encoderList->end())
            {
                iter = encoderList->begin();
            }

            elink = *iter;

            if (retval == -1 && elink->IsConnected() &&
                !elink->IsTunerLocked() &&
                !elink->GetFreeInputs(excluded_cardids).empty())
            {
                encoder = elink;
                retval = iter.key();
            }

            // cycled right through? no more available recorders
            if (iter == curr)
                break;
        }
    }
    else
    {
        HandleGetFreeRecorder(pbs);
        return;
    }


    strlist << QString::number(retval);

    if (encoder)
    {
        if (encoder->IsLocal())
        {
            strlist << gContext->GetSetting("BackendServerIP");
            strlist << gContext->GetSetting("BackendServerPort");
        }
        else
        {
            strlist << gContext->GetSettingOnHost("BackendServerIP",
                                                  encoder->GetHostName(),
                                                  "nohostname");
            strlist << gContext->GetSettingOnHost("BackendServerPort",
                                                  encoder->GetHostName(), "-1");
        }
    }
    else
    {
        strlist << "nohost";
        strlist << "-1";
    }

    SendResponse(pbssock, strlist);
}

static QString cleanup(const QString &str)
{
    if (str == " ")
        return "";
    return str;
}

static QString make_safe(const QString &str)
{
    if (str.isEmpty())
        return " ";
    return str;
}

void MainServer::HandleRecorderQuery(QStringList &slist, QStringList &commands,
                                     PlaybackSock *pbs)
{
    MythSocket *pbssock = pbs->getSocket();

    if (commands.size() < 2 || slist.size() < 2)
        return;
    
    int recnum = commands[1].toInt();

    QMap<int, EncoderLink *>::Iterator iter = encoderList->find(recnum);
    if (iter == encoderList->end())
    {
        VERBOSE(VB_IMPORTANT, "MainServer::HandleRecorderQuery() " +
                QString("Unknown encoder: %1").arg(recnum));
        QStringList retlist( "bad" );
        SendResponse(pbssock, retlist);
        return;
    }

    QString command = slist[1];

    QStringList retlist;

    EncoderLink *enc = *iter;
    if (!enc->IsConnected())
    {
        VERBOSE(VB_IMPORTANT," MainServer::HandleRecorderQuery() " +
                QString("Command %1 for unconnected encoder %2")
                .arg(command).arg(recnum));
        retlist << "bad";
        SendResponse(pbssock, retlist);
        return;
    }

    if (command == "IS_RECORDING")
    {
        retlist << QString::number((int)enc->IsReallyRecording());
    }
    else if (command == "GET_FRAMERATE")
    {
        retlist << QString::number(enc->GetFramerate());
    }
    else if (command == "GET_FRAMES_WRITTEN")
    {
        long long value = enc->GetFramesWritten();
        encodeLongLong(retlist, value);
    }
    else if (command == "GET_FILE_POSITION")
    {
        long long value = enc->GetFilePosition();
        encodeLongLong(retlist, value);
    }
    else if (command == "GET_MAX_BITRATE")
    {
        long long value = enc->GetMaxBitrate();
        encodeLongLong(retlist, value);
    }
    else if (command == "GET_CURRENT_RECORDING")
    {
        ProgramInfo *info = enc->GetRecording();
        if (info)
        {
            info->ToStringList(retlist);
            delete info;
        }
        else
        {
            ProgramInfo dummy;
            dummy.ToStringList(retlist);
        }
    }
    else if (command == "GET_KEYFRAME_POS")
    {
        long long desired = decodeLongLong(slist, 2);

        long long value = enc->GetKeyframePosition(desired);
        encodeLongLong(retlist, value);
    }
    else if (command == "FILL_POSITION_MAP")
    {
        long long start = slist[2].toLongLong();
        long long end   = slist[3].toLongLong();
        PosMap map;

        if (!enc->GetKeyframePositions(start, end, map))
        {
            retlist << "error";
        }
        else
        {
            PosMap::const_iterator it = map.begin();
            for (; it != map.end(); ++it)
            {
                retlist += QString::number(it.key());
                retlist += QString::number(*it);
            }
            if (retlist.empty())
                retlist << "ok";
        }
    }
    else if (command == "GET_RECORDING")
    {
        ProgramInfo *pginfo = enc->GetRecording();
        if (pginfo)
        {
            pginfo->ToStringList(retlist);
            delete pginfo;
        }
        else
        {
            ProgramInfo dummy;
            dummy.ToStringList(retlist);
        }
    }
    else if (command == "FRONTEND_READY")
    {
        enc->FrontendReady();
        retlist << "ok";
    }
    else if (command == "CANCEL_NEXT_RECORDING")
    {
        QString cancel = slist[2];
        VERBOSE(VB_IMPORTANT, "Received: CANCEL_NEXT_RECORDING "<<cancel);
        enc->CancelNextRecording(cancel == "1");
        retlist << "ok";
    }
    else if (command == "SPAWN_LIVETV")
    {
        QString chainid = slist[2];
        LiveTVChain *chain = GetExistingChain(chainid);
        if (!chain)
        {
            chain = new LiveTVChain();
            chain->LoadFromExistingChain(chainid);
            AddToChains(chain);
        }

        chain->SetHostSocket(pbssock);

        enc->SpawnLiveTV(chain, slist[3].toInt(), slist[4]);
        retlist << "ok";
    }
    else if (command == "STOP_LIVETV")
    {
        QString chainid = enc->GetChainID();
        enc->StopLiveTV();

        LiveTVChain *chain = GetExistingChain(chainid);
        if (chain)
        {
            chain->DelHostSocket(pbssock);
            if (chain->HostSocketCount() == 0)
            {
                DeleteChain(chain);
            }
        }

        retlist << "ok";
    }
    else if (command == "PAUSE")
    {
        enc->PauseRecorder();
        retlist << "ok";
    }
    else if (command == "FINISH_RECORDING")
    {
        enc->FinishRecording();
        retlist << "ok";
    }
    else if (command == "SET_LIVE_RECORDING")
    {
        int recording = slist[2].toInt();
        enc->SetLiveRecording(recording);
        retlist << "ok";
    }
    else if (command == "GET_FREE_INPUTS")
    {
        vector<uint> excluded_cardids;
        for (int i = 2; i < slist.size(); i++)
            excluded_cardids.push_back(slist[i].toUInt());

        vector<InputInfo> inputs = enc->GetFreeInputs(excluded_cardids);

        if (inputs.empty())
            retlist << "EMPTY_LIST";
        else
        {
            for (uint i = 0; i < inputs.size(); i++)
                inputs[i].ToStringList(retlist);
        }
    }
    else if (command == "GET_INPUT")
    {
        QString ret = enc->GetInput();
        ret = (ret.isEmpty()) ? "UNKNOWN" : ret;
        retlist << ret;
    }
    else if (command == "SET_INPUT")
    {
        QString input = slist[2];
        QString ret   = enc->SetInput(input);
        ret = (ret.isEmpty()) ? "UNKNOWN" : ret;
        retlist << ret;
    }
    else if (command == "TOGGLE_CHANNEL_FAVORITE")
    {
        QString changroup = slist[2];
        enc->ToggleChannelFavorite(changroup);
        retlist << "ok";
    }
    else if (command == "CHANGE_CHANNEL")
    {
        int direction = slist[2].toInt();
        enc->ChangeChannel(direction);
        retlist << "ok";
    }
    else if (command == "SET_CHANNEL")
    {
        QString name = slist[2];
        enc->SetChannel(name);
        retlist << "ok";
    }
    else if (command == "SET_SIGNAL_MONITORING_RATE")
    {
        int rate = slist[2].toInt();
        int notifyFrontend = slist[3].toInt();
        int oldrate = enc->SetSignalMonitoringRate(rate, notifyFrontend);
        retlist << QString::number(oldrate);
    }
    else if (command == "GET_COLOUR")
    {
        int ret = enc->GetPictureAttribute(kPictureAttribute_Colour);
        retlist << QString::number(ret);
    }
    else if (command == "GET_CONTRAST")
    {
        int ret = enc->GetPictureAttribute(kPictureAttribute_Contrast);
        retlist << QString::number(ret);
    }
    else if (command == "GET_BRIGHTNESS")
    {
        int ret = enc->GetPictureAttribute(kPictureAttribute_Brightness);
        retlist << QString::number(ret);
    }
    else if (command == "GET_HUE")
    {
        int ret = enc->GetPictureAttribute(kPictureAttribute_Hue);
        retlist << QString::number(ret);
    }
    else if (command == "CHANGE_COLOUR")
    {
        int  type = slist[2].toInt();
        bool up   = slist[3].toInt();
        int  ret = enc->ChangePictureAttribute(
            (PictureAdjustType) type, kPictureAttribute_Colour, up);
        retlist << QString::number(ret);
    }
    else if (command == "CHANGE_CONTRAST")
    {
        int  type = slist[2].toInt();
        bool up   = slist[3].toInt();
        int  ret = enc->ChangePictureAttribute(
            (PictureAdjustType) type, kPictureAttribute_Contrast, up);
        retlist << QString::number(ret);
    }
    else if (command == "CHANGE_BRIGHTNESS")
    {
        int  type= slist[2].toInt();
        bool up  = slist[3].toInt();
        int  ret = enc->ChangePictureAttribute(
            (PictureAdjustType) type, kPictureAttribute_Brightness, up);
        retlist << QString::number(ret);
    }
    else if (command == "CHANGE_HUE")
    {
        int  type= slist[2].toInt();
        bool up  = slist[3].toInt();
        int  ret = enc->ChangePictureAttribute(
            (PictureAdjustType) type, kPictureAttribute_Hue, up);
        retlist << QString::number(ret);
    }
    else if (command == "CHECK_CHANNEL")
    {
        QString name = slist[2];
        retlist << QString::number((int)(enc->CheckChannel(name)));
    }
    else if (command == "SHOULD_SWITCH_CARD")
    {
        QString chanid = slist[2];
        retlist << QString::number((int)(enc->ShouldSwitchToAnotherCard(chanid)));
    }
    else if (command == "CHECK_CHANNEL_PREFIX")
    {
        QString needed_spacer = QString::null;
        QString prefix        = slist[2];
        uint    is_complete_valid_channel_on_rec = 0;
        bool    is_extra_char_useful             = false;

        bool match = enc->CheckChannelPrefix(
            prefix, is_complete_valid_channel_on_rec,
            is_extra_char_useful, needed_spacer);

        retlist << QString::number((int)match);
        retlist << QString::number(is_complete_valid_channel_on_rec);
        retlist << QString::number((int)is_extra_char_useful);
        retlist << ((needed_spacer.isEmpty()) ? QString("X") : needed_spacer);
    }
    else if (command == "GET_NEXT_PROGRAM_INFO")
    {
        QString channelname = slist[2];
        QString chanid = slist[3];
        int direction = slist[4].toInt();
        QString starttime = slist[5];

        QString title = "", subtitle = "", desc = "", category = "";
        QString endtime = "", callsign = "", iconpath = "";
        QString seriesid = "", programid = "";

        enc->GetNextProgram(direction,
                            title, subtitle, desc, category, starttime,
                            endtime, callsign, iconpath, channelname, chanid,
                            seriesid, programid);

        retlist << make_safe(title);
        retlist << make_safe(subtitle);
        retlist << make_safe(desc);
        retlist << make_safe(category);
        retlist << make_safe(starttime);
        retlist << make_safe(endtime);
        retlist << make_safe(callsign);
        retlist << make_safe(iconpath);
        retlist << make_safe(channelname);
        retlist << make_safe(chanid);
        retlist << make_safe(seriesid);
        retlist << make_safe(programid);
    }
    else if (command == "GET_CHANNEL_INFO")
    {
        uint chanid = slist[2].toUInt();
        uint sourceid = 0;
        QString callsign = "", channum = "", channame = "", xmltv = "";

        enc->GetChannelInfo(chanid, sourceid,
                            callsign, channum, channame, xmltv);

        retlist << QString::number(chanid);
        retlist << QString::number(sourceid);
        retlist << make_safe(callsign);
        retlist << make_safe(channum);
        retlist << make_safe(channame);
        retlist << make_safe(xmltv);
    }
    else
    {
        VERBOSE(VB_IMPORTANT, QString("Unknown command: %1").arg(command));
        retlist << "ok";
    }

    SendResponse(pbssock, retlist);
}

void MainServer::HandleSetNextLiveTVDir(QStringList &commands,
                                        PlaybackSock *pbs)
{
    MythSocket *pbssock = pbs->getSocket();

    int recnum = commands[1].toInt();

    QMap<int, EncoderLink *>::Iterator iter = encoderList->find(recnum);
    if (iter == encoderList->end())
    {
        VERBOSE(VB_IMPORTANT, "MainServer::HandleSetNextLiveTVDir() " +
                QString("Unknown encoder: %1").arg(recnum));
        QStringList retlist( "bad" );
        SendResponse(pbssock, retlist);
        return;
    }

    EncoderLink *enc = *iter;
    enc->SetNextLiveTVDir(commands[2]);

    QStringList retlist( "OK" );
    SendResponse(pbssock, retlist);
}

void MainServer::HandleSetChannelInfo(QStringList &slist, PlaybackSock *pbs)
{
    bool        ok       = true;
    MythSocket *pbssock  = pbs->getSocket();
    uint        chanid   = slist[1].toUInt();
    uint        sourceid = slist[2].toUInt();
    QString     oldcnum  = cleanup(slist[3]);
    QString     callsign = cleanup(slist[4]);
    QString     channum  = cleanup(slist[5]);
    QString     channame = cleanup(slist[6]);
    QString     xmltv    = cleanup(slist[7]);

    QStringList retlist;
    if (!chanid || !sourceid)
    {
        retlist << "0";
        SendResponse(pbssock, retlist);
        return;
    }

    QMap<int, EncoderLink *>::iterator it = encoderList->begin();
    for (; it != encoderList->end(); ++it)
    {
        if (*it)
        {
            ok &= (*it)->SetChannelInfo(chanid, sourceid, oldcnum,
                                        callsign, channum, channame, xmltv);
        }
    }

    retlist << ((ok) ? "1" : "0");
    SendResponse(pbssock, retlist);
}

void MainServer::HandleRemoteEncoder(QStringList &slist, QStringList &commands,
                                     PlaybackSock *pbs)
{
    MythSocket *pbssock = pbs->getSocket();

    int recnum = commands[1].toInt();
    QStringList retlist;

    QMap<int, EncoderLink *>::Iterator iter = encoderList->find(recnum);
    if (iter == encoderList->end())
    {
        VERBOSE(VB_IMPORTANT, "MainServer: " +
                QString("HandleRemoteEncoder(cmd %1) ").arg(slist[1]) +
                QString("Unknown encoder: %1").arg(recnum));
        retlist << QString::number((int) kState_Error);
        SendResponse(pbssock, retlist);
        return;
    }

    EncoderLink *enc = *iter;

    QString command = slist[1];

    if (command == "GET_STATE")
    {
        retlist << QString::number((int)enc->GetState());
    }
    else if (command == "GET_SLEEPSTATUS")
    {
        retlist << QString::number(enc->GetSleepStatus());
    }
    else if (command == "GET_FLAGS")
    {
        retlist << QString::number(enc->GetFlags());
    }
    else if (command == "IS_BUSY")
    {
        int time_buffer = (slist.size() >= 3) ? slist[2].toInt() : 5;
        TunedInputInfo busy_input;
        retlist << QString::number((int)enc->IsBusy(&busy_input, time_buffer));
        busy_input.ToStringList(retlist);
    }
    else if (command == "MATCHES_RECORDING")
    {
        ProgramInfo *pginfo = new ProgramInfo();
        pginfo->FromStringList(slist, 2);

        retlist << QString::number((int)enc->MatchesRecording(pginfo));

        delete pginfo;
    }
    else if (command == "START_RECORDING")
    {
        ProgramInfo *pginfo = new ProgramInfo();
        pginfo->FromStringList(slist, 2);

        retlist << QString::number(enc->StartRecording(pginfo));

        delete pginfo;
    }
    else if (command == "RECORD_PENDING")
    {
        ProgramInfo *pginfo = new ProgramInfo();
        int secsleft = slist[2].toInt();
        int haslater = slist[3].toInt();
        pginfo->FromStringList(slist, 4);

        enc->RecordPending(pginfo, secsleft, haslater);

        retlist << "OK";
        delete pginfo;
    }
    else if (command == "CANCEL_NEXT_RECORDING")
    {
        bool cancel = (bool) slist[2].toInt();
        enc->CancelNextRecording(cancel);
        retlist << "OK";
    }
    else if (command == "STOP_RECORDING")
    {
        enc->StopRecording();
        retlist << "OK";
    }
    else if (command == "GET_MAX_BITRATE")
    {
        long long value = enc->GetMaxBitrate();
        encodeLongLong(retlist, value);
    }
    else if (command == "GET_CURRENT_RECORDING")
    {
        ProgramInfo *info = enc->GetRecording();
        if (info)
        {
            info->ToStringList(retlist);
            delete info;
        }
        else
        {
            ProgramInfo dummy;
            dummy.ToStringList(retlist);
        }
    }
    else if (command == "GET_FREE_INPUTS")
    {
        vector<uint> excluded_cardids;
        for (int i = 2; i < slist.size(); i++)
            excluded_cardids.push_back(slist[i].toUInt());

        vector<InputInfo> inputs = enc->GetFreeInputs(excluded_cardids);

        if (inputs.empty())
            retlist << "EMPTY_LIST";
        else
        {
            for (uint i = 0; i < inputs.size(); i++)
                inputs[i].ToStringList(retlist);
        }
    }

    SendResponse(pbssock, retlist);
}

void MainServer::HandleIsActiveBackendQuery(QStringList &slist,
                                            PlaybackSock *pbs)
{
    QStringList retlist;
    QString queryhostname = slist[1];

    if (gContext->GetHostName() != queryhostname)
    {
        PlaybackSock *slave = getSlaveByHostname(queryhostname);
        if (slave != NULL)
        {
            retlist << "TRUE";
            slave->DownRef();
        }
        else
            retlist << "FALSE";
    }
    else
        retlist << "TRUE";

    SendResponse(pbs->getSocket(), retlist);
}

void *MainServer::SpawnTruncateThread(void *param)
{
    DeleteStruct *ds = (DeleteStruct *)param;

    MainServer *ms = ds->ms;
    ms->DoTruncateThread(ds);

    delete ds;

    return NULL;
}

void MainServer::DoTruncateThread(const DeleteStruct *ds)
{
    if (gContext->GetNumSetting("TruncateDeletesSlowly", 0))
        TruncateAndClose(NULL, ds->fd, ds->filename, ds->size);
    else
    {
        QMutexLocker dl(&deletelock);
        close(ds->fd);
    }
}

bool MainServer::HandleDeleteFile(QStringList &slist, PlaybackSock *pbs)
{
    return HandleDeleteFile(slist[1], slist[2], pbs);
}

bool MainServer::HandleDeleteFile(QString filename, QString storagegroup,
                                  PlaybackSock *pbs)
{
    StorageGroup sgroup(storagegroup, "", false);
    QStringList retlist;

    if ((filename.isEmpty()) ||
        (filename.contains("/../")) ||
        (filename.startsWith("../")))
    {
        VERBOSE(VB_IMPORTANT, QString("ERROR deleting file, filename '%1' "
                "fails sanity checks").arg(filename));
        if (pbs)
        {
            retlist << "0";
            SendResponse(pbs->getSocket(), retlist);
        }
        return false;
    }

    QString fullfile = sgroup.FindRecordingFile(filename);

    if (fullfile.isEmpty()) {
        VERBOSE(VB_IMPORTANT, QString("Unable to find %1 in HandleDeleteFile()")
                .arg(filename));
        if (pbs)
        {
            retlist << "0";
            SendResponse(pbs->getSocket(), retlist);
        }
        return false;
    }

    QFile checkFile(fullfile);
    bool followLinks = gContext->GetNumSetting("DeletesFollowLinks", 0);
    int fd = -1;
    off_t size = 0;

    // This will open the file and unlink the dir entry.  The actual file
    // data will be deleted in the truncate thread spawned below.
    // Since stat fails after unlinking on some filesystems, get the size first
    const QFileInfo info(fullfile);
    size = info.size();
    fd = DeleteFile(fullfile, followLinks);

    if ((fd < 0) && checkFile.exists())
    {
        VERBOSE(VB_IMPORTANT, QString("Error deleting file: %1.")
                .arg(fullfile));
        if (pbs)
        {
            retlist << "0";
            SendResponse(pbs->getSocket(), retlist);
        }
        return false;
    }

    if (pbs)
    {
        retlist << "1";
        SendResponse(pbs->getSocket(), retlist);
    }

    // DeleteFile() opened up a file for us to delete
    if (fd >= 0)
    {
        // Thread off the actual file delete
        DeleteStruct *ds = new DeleteStruct;
        ds->ms = this;
        ds->filename = fullfile;
        ds->fd = fd;
        ds->size = size;

        pthread_t truncateThread;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&truncateThread, &attr, SpawnTruncateThread, ds);
        pthread_attr_destroy(&attr);
    }

    return true;
}

// Helper function for the guts of HandleCommBreakQuery + HandleCutListQuery
void MainServer::HandleCutMapQuery(const QString &chanid,
                                   const QString &starttime,
                                   PlaybackSock *pbs, bool commbreak)
{
    MythSocket *pbssock = NULL;
    if (pbs)
        pbssock = pbs->getSocket();

    QMap<long long, int> markMap;
    QMap<long long, int>::Iterator it;
    QDateTime startdt;
    startdt.setTime_t(starttime.toULongLong());
    QStringList retlist;
    int rowcnt = 0;

    ProgramInfo *pginfo = ProgramInfo::GetProgramFromRecorded(chanid,
                                                              startdt);

    if (pginfo)
    {
        if (commbreak)
            pginfo->GetCommBreakList(markMap);
        else
            pginfo->GetCutList(markMap);
        delete pginfo;

        for (it = markMap.begin(); it != markMap.end(); ++it)
        {
            rowcnt++;
            QString intstr = QString("%1").arg(*it);
            retlist << intstr;
            encodeLongLong(retlist,it.key());
        }
    }

    if (rowcnt > 0)
        retlist.prepend(QString("%1").arg(rowcnt));
    else
        retlist << "-1";

    if (pbssock)
        SendResponse(pbssock, retlist);

    return;
}

void MainServer::HandleCommBreakQuery(const QString &chanid,
                                      const QString &starttime,
                                      PlaybackSock *pbs)
{
// Commercial break query
// Format:  QUERY_COMMBREAK <chanid> <starttime>
// chanid is chanid, starttime is startime of program in
//   # of seconds since Jan 1, 1970, in UTC time.  Same format as in
//   a ProgramInfo structure in a string list.
// Return structure is [number of rows] followed by a triplet of values:
//   each triplet : [type] [long portion 1] [long portion 2]
// type is the value in the map, right now 4 = commbreak start, 5= end
    return HandleCutMapQuery(chanid, starttime, pbs, true);
}

void MainServer::HandleCutlistQuery(const QString &chanid,
                                    const QString &starttime,
                                    PlaybackSock *pbs)
{
// Cutlist query
// Format:  QUERY_CUTLIST <chanid> <starttime>
// chanid is chanid, starttime is startime of program in
//   # of seconds since Jan 1, 1970, in UTC time.  Same format as in
//   a ProgramInfo structure in a string list.
// Return structure is [number of rows] followed by a triplet of values:
//   each triplet : [type] [long portion 1] [long portion 2]
// type is the value in the map, right now 0 = commbreak start, 1 = end
    return HandleCutMapQuery(chanid, starttime, pbs, false);
}


void MainServer::HandleBookmarkQuery(const QString &chanid,
                                     const QString &starttime,
                                     PlaybackSock *pbs)
// Bookmark query
// Format:  QUERY_BOOKMARK <chanid> <starttime>
// chanid is chanid, starttime is startime of program in
//   # of seconds since Jan 1, 1970, in UTC time.  Same format as in
//   a ProgramInfo structure in a string list.
// Return value is a long-long encoded as two separate values
{
    MythSocket *pbssock = NULL;
    if (pbs)
        pbssock = pbs->getSocket();

    QDateTime startdt;
    startdt.setTime_t(starttime.toULongLong());
    QStringList retlist;
    long long bookmark = 0;

    ProgramInfo *pginfo = ProgramInfo::GetProgramFromRecorded(chanid,
                                                              startdt);
    if (pginfo)
    {
        bookmark = pginfo->GetBookmark();
        delete pginfo;
    }

    encodeLongLong(retlist,bookmark);

    if (pbssock)
        SendResponse(pbssock, retlist);

    return;
}


void MainServer::HandleSetBookmark(QStringList &tokens,
                                   PlaybackSock *pbs)
{
// Bookmark query
// Format:  SET_BOOKMARK <chanid> <starttime> <long part1> <long part2>
// chanid is chanid, starttime is startime of program in
//   # of seconds since Jan 1, 1970, in UTC time.  Same format as in
//   a ProgramInfo structure in a string list.  The two longs are the two
//   portions of the bookmark value to set.

    MythSocket *pbssock = NULL;
    if (pbs)
        pbssock = pbs->getSocket();

    QString chanid = tokens[1];
    QString starttime = tokens[2];
    QStringList bookmarklist;
    bookmarklist << tokens[3];
    bookmarklist << tokens[4];

    QDateTime startdt;
    startdt.setTime_t(starttime.toULongLong());
    QStringList retlist;
    long long bookmark = decodeLongLong(bookmarklist, 0);

    ProgramInfo *pginfo = ProgramInfo::GetProgramFromRecorded(chanid,
                                                              startdt);
    if (pginfo)
    {
        pginfo->SetBookmark(bookmark);
        delete pginfo;
        retlist << "OK";
    }
    else
        retlist << "FAILED";

    if (pbssock)
        SendResponse(pbssock, retlist);

    return;
}

void MainServer::HandleSettingQuery(QStringList &tokens, PlaybackSock *pbs)
{
// Format: QUERY_SETTING <hostname> <setting>
// Returns setting value as a string

    MythSocket *pbssock = NULL;
    if (pbs)
        pbssock = pbs->getSocket();

    QString hostname = tokens[1];
    QString setting = tokens[2];
    QStringList retlist;

    QString retvalue = gContext->GetSettingOnHost(setting, hostname, "-1");

    retlist << retvalue;
    if (pbssock)
        SendResponse(pbssock, retlist);

    return;
}

void MainServer::HandleSetSetting(QStringList &tokens,
                                  PlaybackSock *pbs)
{
// Format: SET_SETTING <hostname> <setting> <value>
    MythSocket *pbssock = NULL;
    if (pbs)
        pbssock = pbs->getSocket();

    QString hostname = tokens[1];
    QString setting = tokens[2];
    QString svalue = tokens[3];
    QStringList retlist;

    if (gContext->SaveSettingOnHost(setting, svalue, hostname))
        retlist << "OK";
    else
        retlist << "ERROR";

    if (pbssock)
        SendResponse(pbssock, retlist);

    return;
}

void MainServer::HandleFileTransferQuery(QStringList &slist,
                                         QStringList &commands,
                                         PlaybackSock *pbs)
{
    MythSocket *pbssock = pbs->getSocket();

    int recnum = commands[1].toInt();
    QString command = slist[1];

    QStringList retlist;

    sockListLock.lockForRead();
    FileTransfer *ft = GetFileTransferByID(recnum);
    if (!ft)
    {
        if (command == "DONE")
        {
            // if there is an error opening the file, we may not have a
            // FileTransfer instance for this connection.
            retlist << "ok";
        }
        else
        {
            VERBOSE(VB_IMPORTANT, QString("Unknown file transfer socket: %1")
                                   .arg(recnum));
            retlist << QString("ERROR: Unknown file transfer socket: %1")
                               .arg(recnum);
        }

        SendResponse(pbssock, retlist);
        sockListLock.unlock();
        return;
    }

    ft->UpRef();
    sockListLock.unlock();

    if (command == "IS_OPEN")
    {
        bool isopen = ft->isOpen();

        retlist << QString::number(isopen);
    }
    else if (command == "DONE")
    {
        ft->Stop();
        retlist << "ok";
    }
    else if (command == "REQUEST_BLOCK")
    {
        int size = slist[2].toInt();

        retlist << QString::number(ft->RequestBlock(size));
    }
    else if (command == "WRITE_BLOCK")
    {
        int size = slist[2].toInt();

        retlist << QString::number(ft->WriteBlock(size));
    }
    else if (command == "SEEK")
    {
        long long pos = decodeLongLong(slist, 2);
        int whence = slist[4].toInt();
        long long curpos = decodeLongLong(slist, 5);

        long long ret = ft->Seek(curpos, pos, whence);
        encodeLongLong(retlist, ret);
    }
    else if (command == "SET_TIMEOUT")
    {
        bool fast = slist[2].toInt();
        ft->SetTimeout(fast);
        retlist << "ok";
    }
    else
    {
        VERBOSE(VB_IMPORTANT, QString("Unknown command: %1").arg(command));
        retlist << "ok";
    }

    ft->DownRef();

    SendResponse(pbssock, retlist);
}

void MainServer::HandleGetRecorderNum(QStringList &slist, PlaybackSock *pbs)
{
    MythSocket *pbssock = pbs->getSocket();

    int retval = -1;

    ProgramInfo *pginfo = new ProgramInfo();
    pginfo->FromStringList(slist, 1);

    EncoderLink *encoder = NULL;

    QMap<int, EncoderLink *>::Iterator iter = encoderList->begin();
    for (; iter != encoderList->end(); ++iter)
    {
        EncoderLink *elink = *iter;

        if (elink->IsConnected() && elink->MatchesRecording(pginfo))
        {
            retval = iter.key();
            encoder = elink;
        }
    }

    QStringList strlist( QString::number(retval) );

    if (encoder)
    {
        if (encoder->IsLocal())
        {
            strlist << gContext->GetSetting("BackendServerIP");
            strlist << gContext->GetSetting("BackendServerPort");
        }
        else
        {
            strlist << gContext->GetSettingOnHost("BackendServerIP",
                                                  encoder->GetHostName(),
                                                  "nohostname");
            strlist << gContext->GetSettingOnHost("BackendServerPort",
                                                  encoder->GetHostName(), "-1");
        }
    }
    else
    {
        strlist << "nohost";
        strlist << "-1";
    }

    SendResponse(pbssock, strlist);
    delete pginfo;
}

void MainServer::HandleGetRecorderFromNum(QStringList &slist,
                                          PlaybackSock *pbs)
{
    MythSocket *pbssock = pbs->getSocket();

    int recordernum = slist[1].toInt();
    EncoderLink *encoder = NULL;
    QStringList strlist;

    QMap<int, EncoderLink *>::Iterator iter = encoderList->find(recordernum);

    if (iter != encoderList->end())
        encoder =  (*iter);

    if (encoder && encoder->IsConnected())
    {
        if (encoder->IsLocal())
        {
            strlist << gContext->GetSetting("BackendServerIP");
            strlist << gContext->GetSetting("BackendServerPort");
        }
        else
        {
            strlist << gContext->GetSettingOnHost("BackendServerIP",
                                                  encoder->GetHostName(),
                                                  "nohostname");
            strlist << gContext->GetSettingOnHost("BackendServerPort",
                                                  encoder->GetHostName(), "-1");
        }
    }
    else
    {
        strlist << "nohost";
        strlist << "-1";
    }

    SendResponse(pbssock, strlist);
}

void MainServer::HandleMessage(QStringList &slist, PlaybackSock *pbs)
{
    if (slist.size() < 2)
        return;

    MythSocket *pbssock = pbs->getSocket();

    QString message = slist[1];
    QStringList extra_data;
    for (uint i = 2; i < (uint) slist.size(); i++)
        extra_data.push_back(slist[i]);

    if (extra_data.empty())
    {
        MythEvent me(message);
        gContext->dispatch(me);
    }
    else
    {
        MythEvent me(message, extra_data);
        gContext->dispatch(me);
    }

    QStringList retlist( "OK" );

    SendResponse(pbssock, retlist);
}

void MainServer::HandleSetVerbose(QStringList &slist, PlaybackSock *pbs)
{
    MythSocket *pbssock = pbs->getSocket();
    QStringList retlist;
    
    QString newverbose = slist[1];
    int len=newverbose.length();
    if (len > 12)
    {
        parse_verbose_arg(newverbose.right(len-12));

        VERBOSE(VB_IMPORTANT, QString("Verbose level changed, new level is: %1")
                                      .arg(verboseString));

        retlist << "OK";
    }
    else
    {
        VERBOSE(VB_IMPORTANT, QString("Invalid SET_VERBOSE string: '%1'")
                                      .arg(newverbose));
        retlist << "Failed";
    }

    SendResponse(pbssock, retlist);
}

void MainServer::HandleIsRecording(QStringList &slist, PlaybackSock *pbs)
{
    (void)slist;

    MythSocket *pbssock = pbs->getSocket();
    int RecordingsInProgress = 0;
    int LiveTVRecordingsInProgress = 0;
    QStringList retlist;

    QMap<int, EncoderLink *>::Iterator iter = encoderList->begin();
    for (; iter != encoderList->end(); ++iter)
    {
        EncoderLink *elink = *iter;
        if (elink->IsBusyRecording()) {
            RecordingsInProgress++;

            ProgramInfo *info = elink->GetRecording();
            if (info && info->recgroup == "LiveTV")
                LiveTVRecordingsInProgress++;

            delete info;
        }
    }

    retlist << QString::number(RecordingsInProgress);
    retlist << QString::number(LiveTVRecordingsInProgress);

    SendResponse(pbssock, retlist);
}

void MainServer::HandleGenPreviewPixmap(QStringList &slist, PlaybackSock *pbs)
{
    MythSocket *pbssock = pbs->getSocket();

    bool      time_fmt_sec   = true;
    long long time           = -1;
    QString   outputfile     = QString::null;
    int       width          = -1;
    int       height         = -1;
    bool      has_extra_data = false;

    QStringList::const_iterator it = slist.begin() + 1;
    QStringList::const_iterator end = slist.end();
    ProgramInfo *pginfo = new ProgramInfo();
    bool ok = pginfo->FromStringList(it, end);
    if (!ok)
    {
        VERBOSE(VB_IMPORTANT, "MainServer: Failed to parse pixmap request.");
        QStringList outputlist("BAD");
        outputlist += "ERROR_INVALID_REQUEST";
        SendResponse(pbssock, outputlist);
    }
    if (it != slist.end())
        (time_fmt_sec = ((*it).toLower() == "s")), it++;
    if (it != slist.end())
        time = decodeLongLong(slist, it);
    if (it != slist.end())
        (outputfile = *it), it++;
    outputfile = (outputfile == "<EMPTY>") ? QString::null : outputfile;
    if (it != slist.end())
    {
        width = (*it).toInt(&ok); it++;
        width = (ok) ? width : -1;
    }
    if (it != slist.end())
    {
        height = (*it).toInt(&ok); it++;
        height = (ok) ? height : -1;
        has_extra_data = true;
    }
    QSize outputsize = QSize(width, height);

    if (has_extra_data)
    {
        VERBOSE(VB_PLAYBACK, "HandleGenPreviewPixmap got extra data\n\t\t\t"
                << QString("%1%2 %3x%4 '%5'")
                .arg(time).arg(time_fmt_sec?"s":"f")
                .arg(width).arg(height).arg(outputfile));
    }

    pginfo->pathname = GetPlaybackURL(pginfo);

    if ((ismaster) &&
        (pginfo->hostname != gContext->GetHostName()) &&
        ((!masterBackendOverride) ||
         (pginfo->pathname.left(1) != "/")))
    {
        PlaybackSock *slave = getSlaveByHostname(pginfo->hostname);

        if (slave)
        {
            QStringList outputlist("OK");
            if (has_extra_data)
            {
                outputlist = slave->GenPreviewPixmap(
                    pginfo, time_fmt_sec, time, outputfile, outputsize);
            }
            else
            {
                outputlist = slave->GenPreviewPixmap(pginfo);
            }

            slave->DownRef();
            SendResponse(pbssock, outputlist);
            delete pginfo;
            return;
        }
        VERBOSE(VB_IMPORTANT, "MainServer::HandleGenPreviewPixmap()"
                "\n\t\t\tCouldn't find backend for: " +
                QString("\n\t\t\t%1 : \"%2\"")
                .arg(pginfo->title).arg(pginfo->subtitle));
    }

    if (pginfo->pathname.left(1) != "/")
    {
        VERBOSE(VB_IMPORTANT, "MainServer: HandleGenPreviewPixmap: Unable to "
                "find file locally, unable to make preview image.");
        QStringList outputlist( "BAD" );
        outputlist += "ERROR_NOFILE";
        SendResponse(pbssock, outputlist);
        delete pginfo;
        return;
    }

    PreviewGenerator *previewgen = new PreviewGenerator(pginfo);
    if (has_extra_data)
    {
        previewgen->SetOutputSize(outputsize);
        previewgen->SetOutputFilename(outputfile);
        previewgen->SetPreviewTime(time, time_fmt_sec);
    }
    ok = previewgen->Run();
    previewgen->deleteLater();

    if (ok)
    {
        QStringList outputlist("OK");
        if (!outputfile.isEmpty())
            outputlist += outputfile;
        SendResponse(pbssock, outputlist);
    }
    else
    {
        VERBOSE(VB_IMPORTANT, "MainServer: Failed to make preview image.");
        QStringList outputlist( "BAD" );
        outputlist += "ERROR_UNKNOWN";
        SendResponse(pbssock, outputlist);
    }

    delete pginfo;
}

void MainServer::HandlePixmapLastModified(QStringList &slist, PlaybackSock *pbs)
{
    MythSocket *pbssock = pbs->getSocket();

    ProgramInfo *pginfo = new ProgramInfo();
    pginfo->FromStringList(slist, 1);
    pginfo->pathname = GetPlaybackURL(pginfo);

    QDateTime lastmodified;
    QStringList strlist;

    if ((ismaster) &&
        (pginfo->hostname != gContext->GetHostName()) &&
        ((!masterBackendOverride) ||
         (pginfo->pathname.left(1) != "/")))
    {
        PlaybackSock *slave = getSlaveByHostname(pginfo->hostname);

        if (slave)
        {
             QDateTime slavetime = slave->PixmapLastModified(pginfo);
             slave->DownRef();

             strlist = (slavetime.isValid()) ?
                 QStringList(QString::number(slavetime.toTime_t())) :
                 QStringList("BAD");

             SendResponse(pbssock, strlist);
             delete pginfo;
             return;
        }
        else
        {
            VERBOSE(VB_IMPORTANT, QString("MainServer::HandlePixmapLastModified()"
                    " - Couldn't find backend for: %1 : \"%2\"").arg(pginfo->title)
                    .arg(pginfo->subtitle));
        }
    }

    if (pginfo->pathname.left(1) != "/")
    {
        VERBOSE(VB_IMPORTANT, "MainServer: HandlePixmapLastModified: Unable to "
                "find file locally, unable to get last modified date.");
        QStringList outputlist( "BAD" );
        SendResponse(pbssock, outputlist);
        delete pginfo;
        return;
    }

    QString filename = pginfo->pathname + ".png";

    QFileInfo finfo(filename);

    if (finfo.exists())
    {
        lastmodified = finfo.lastModified();
        strlist = QStringList(QString::number(lastmodified.toTime_t()));
    }
    else
        strlist = QStringList( "BAD" );

    SendResponse(pbssock, strlist);
    delete pginfo;
}

void MainServer::HandlePixmapGetIfModified(
    const QStringList &slist, PlaybackSock *pbs)
{
    QStringList strlist;

    MythSocket *pbssock = pbs->getSocket();
    if (slist.size() < (3 + NUMPROGRAMLINES))
    {
        strlist = QStringList("ERROR");
        strlist += "1: Parameter list too short";
        SendResponse(pbssock, strlist);
        return;
    }

    QDateTime cachemodified;
    if (slist[1].toInt() != -1)
        cachemodified.setTime_t(slist[1].toInt());

    int max_file_size = slist[2].toInt();

    ProgramInfo pginfo;

    if (!pginfo.FromStringList(slist, 3))
    {
        strlist = QStringList("ERROR");
        strlist += "2: Invalid ProgramInfo";
        SendResponse(pbssock, strlist);
        return;
    }

    pginfo.pathname = GetPlaybackURL(&pginfo) + ".png";
    if (pginfo.pathname.left(1) == "/")
    {
        QFileInfo finfo(pginfo.pathname);
        if (finfo.exists())
        {
            size_t fsize = finfo.size();
            QDateTime lastmodified = finfo.lastModified();
            bool out_of_date = !cachemodified.isValid() ||
                (lastmodified > cachemodified);

            if (out_of_date && (fsize > 0) && ((ssize_t)fsize < max_file_size))
            {
                QByteArray data;
                QFile file(pginfo.pathname);
                bool open_ok = file.open(QIODevice::ReadOnly);
                if (open_ok)
                    data = file.readAll();

                if (data.size())
                {
                    VERBOSE(VB_FILE, QString("Read preview file '%1'")
                            .arg(pginfo.pathname));
                    strlist += QString::number(lastmodified.toTime_t());
                    strlist += QString::number(data.size());
                    strlist += QString::number(
                        qChecksum(data.constData(), data.size()));
                    strlist += QString(data.toBase64());
                }
                else
                {
                    VERBOSE(VB_IMPORTANT,
                            QString("Failed to read preview file '%1'")
                            .arg(pginfo.pathname));

                    strlist = QStringList("ERROR");
                    strlist +=
                        QString("3: Failed to read preview file '%1'%2")
                        .arg(pginfo.pathname)
                        .arg((open_ok) ? "" : " open failed");
                }
            }
            else if (out_of_date && (max_file_size > 0))
            {
                if (fsize >= (size_t) max_file_size)
                {
                    strlist = QStringList("WARNING");
                    strlist += QString("1: Preview file too big %1 > %2")
                        .arg(fsize).arg(max_file_size);
                }
                else
                {
                    strlist = QStringList("ERROR");
                    strlist += "4: Preview file is invalid";
                }
            }
            else
            {
                strlist += QString::number(lastmodified.toTime_t());
            }

            SendResponse(pbssock, strlist);
            return;
        }
    }

    // handle remote ...
    if (ismaster && pginfo.hostname != gContext->GetHostName())
    {
        PlaybackSock *slave = getSlaveByHostname(pginfo.hostname);
        if (!slave)
        {
            strlist = QStringList("ERROR");
            strlist +=
                "5: Could not locate mythbackend that made this recording";
            SendResponse(pbssock, strlist);
            return;
        }

        strlist = slave->ForwardRequest(slist);

        slave->DownRef(); slave = NULL;

        if (!strlist.empty())
        {
            SendResponse(pbssock, strlist);
            return;
        }
    }

    strlist = QStringList("WARNING");
    strlist += "2: Could not locate requested file";
    SendResponse(pbssock, strlist);
}

void MainServer::HandleBackendRefresh(MythSocket *socket)
{
    gContext->RefreshBackendConfig();

    QStringList retlist( "OK" );
    SendResponse(socket, retlist);
}

void MainServer::HandleBlockShutdown(bool blockShutdown, PlaybackSock *pbs)
{
    pbs->setBlockShutdown(blockShutdown);

    MythSocket *socket = pbs->getSocket();
    QStringList retlist( "OK" );
    SendResponse(socket, retlist);
}

void MainServer::deferredDeleteSlot(void)
{
    QMutexLocker lock(&deferredDeleteLock);

    if (deferredDeleteList.empty())
        return;

    DeferredDeleteStruct dds = deferredDeleteList.front();
    while (dds.ts.secsTo(QDateTime::currentDateTime()) > 30)
    {
        delete dds.sock;
        deferredDeleteList.pop_front();
        if (deferredDeleteList.empty())
            return;
        dds = deferredDeleteList.front();
    }
}

void MainServer::DeletePBS(PlaybackSock *sock)
{
    DeferredDeleteStruct dds;
    dds.sock = sock;
    dds.ts = QDateTime::currentDateTime();

    QMutexLocker lock(&deferredDeleteLock);
    deferredDeleteList.push_back(dds);
}

void MainServer::connectionClosed(MythSocket *socket)
{
    sockListLock.lockForWrite();

    vector<PlaybackSock *>::iterator it = playbackList.begin();
    for (; it != playbackList.end(); ++it)
    {
        PlaybackSock *pbs = (*it);
        MythSocket *sock = pbs->getSocket();
        if (sock == socket && pbs == masterServer)
        {
            playbackList.erase(it);
            sockListLock.unlock();
            masterServer->DownRef();
            masterServer = NULL;
            MythEvent me("LOCAL_RECONNECT_TO_MASTER");
            gContext->dispatch(me);
            return;
        }
        else if (sock == socket)
        {
            if (ismaster && pbs->isSlaveBackend())
            {
                VERBOSE(VB_IMPORTANT,QString("Slave backend: %1 no longer connected")
                                       .arg(pbs->getHostname()));

                bool isFallingAsleep = true;
                QMap<int, EncoderLink *>::Iterator iter = encoderList->begin();
                for (; iter != encoderList->end(); ++iter)
                {
                    EncoderLink *elink = *iter;
                    if (elink->GetSocket() == pbs)
                    {
                        if (!elink->IsFallingAsleep())
                            isFallingAsleep = false;

                        elink->SetSocket(NULL);
                        if (m_sched)
                            m_sched->SlaveDisconnected(elink->GetCardID());
                    }
                }
                if (m_sched && !isFallingAsleep)
                    m_sched->Reschedule(0);

                QString message = QString("LOCAL_SLAVE_BACKEND_OFFLINE %1")
                                          .arg(pbs->getHostname());
                MythEvent me(message);
                gContext->dispatch(me);

                MythEvent me2("RECORDING_LIST_CHANGE");
                gContext->dispatch(me2);

                SendMythSystemEvent(QString("SLAVE_DISCONNECTED HOSTNAME %1")
                                    .arg(pbs->getHostname()));
            }
            else if (ismaster && pbs->getHostname() != "tzcheck")
            {
                SendMythSystemEvent(QString("CLIENT_DISCONNECTED HOSTNAME %1")
                                    .arg(pbs->getHostname()));
            }

            LiveTVChain *chain;
            if ((chain = GetExistingChain(sock)))
            {
                chain->DelHostSocket(sock);
                if (chain->HostSocketCount() == 0)
                {
                    QMap<int, EncoderLink *>::iterator it =
                        encoderList->begin();
                    for (; it != encoderList->end(); ++it)
                    {
                        EncoderLink *enc = *it;
                        if (enc->IsLocal())
                        {
                            while (enc->GetState() == kState_ChangingState)
                                usleep(500);

                            if (enc->IsBusy() &&
                                enc->GetChainID() == chain->GetID())
                            {
                                enc->StopLiveTV();
                            }
                        }
                    }
                    DeleteChain(chain);
                }
            }

            pbs->SetDisconnected();
            playbackList.erase(it);

            PlaybackSock *testsock = GetPlaybackBySock(socket);
            if (testsock)
                VERBOSE(VB_IMPORTANT, "Playback sock still exists?");

            sockListLock.unlock();

            pbs->DownRef();
            return;
        }
    }

    vector<FileTransfer *>::iterator ft = fileTransferList.begin();
    for (; ft != fileTransferList.end(); ++ft)
    {
        MythSocket *sock = (*ft)->getSocket();
        if (sock == socket)
        {
            (*ft)->DownRef();
            fileTransferList.erase(ft);
            sockListLock.unlock();
            return;
        }
    }

    sockListLock.unlock();

    VERBOSE(VB_IMPORTANT, LOC_WARN +
            QString("Unknown socket closing MythSocket(0x%1)")
            .arg((uint64_t)socket,0,16));
}

PlaybackSock *MainServer::getSlaveByHostname(QString &hostname)
{
    if (!ismaster)
        return NULL;

    sockListLock.lockForRead();

    vector<PlaybackSock *>::iterator iter = playbackList.begin();
    for (; iter != playbackList.end(); iter++)
    {
        PlaybackSock *pbs = *iter;
        if (pbs->isSlaveBackend() &&
            ((pbs->getHostname().toLower() == hostname.toLower()) ||
             (pbs->getIP() == hostname)))
        {
            sockListLock.unlock();
            pbs->UpRef();
            return pbs;
        }
    }

    sockListLock.unlock();

    return NULL;
}

/// Warning you must hold a sockListLock lock before calling this
PlaybackSock *MainServer::GetPlaybackBySock(MythSocket *sock)
{
    PlaybackSock *retval = NULL;

    vector<PlaybackSock *>::iterator it = playbackList.begin();
    for (; it != playbackList.end(); ++it)
    {
        if (sock == (*it)->getSocket())
        {
            retval = (*it);
            break;
        }
    }

    return retval;
}

/// Warning you must hold a sockListLock lock before calling this
FileTransfer *MainServer::GetFileTransferByID(int id)
{
    FileTransfer *retval = NULL;

    vector<FileTransfer *>::iterator it = fileTransferList.begin();
    for (; it != fileTransferList.end(); ++it)
    {
        if (id == (*it)->getSocket()->socket())
        {
            retval = (*it);
            break;
        }
    }

    return retval;
}

/// Warning you must hold a sockListLock lock before calling this
FileTransfer *MainServer::GetFileTransferBySock(MythSocket *sock)
{
    FileTransfer *retval = NULL;

    vector<FileTransfer *>::iterator it = fileTransferList.begin();
    for (; it != fileTransferList.end(); ++it)
    {
        if (sock == (*it)->getSocket())
        {
            retval = (*it);
            break;
        }
    }

    return retval;
}

LiveTVChain *MainServer::GetExistingChain(const QString &id)
{
    QMutexLocker lock(&liveTVChainsLock);

    vector<LiveTVChain*>::iterator it = liveTVChains.begin();
    for (; it != liveTVChains.end(); ++it)
    {
        if ((*it)->GetID() == id)
            return *it;
    }

    return NULL;
}

LiveTVChain *MainServer::GetExistingChain(const MythSocket *sock)
{
    QMutexLocker lock(&liveTVChainsLock);

    vector<LiveTVChain*>::iterator it = liveTVChains.begin();
    for (; it != liveTVChains.end(); ++it)
    {
        if ((*it)->IsHostSocket(sock))
            return *it;
    }

    return NULL;
}

LiveTVChain *MainServer::GetChainWithRecording(const ProgramInfo &pginfo)
{
    QMutexLocker lock(&liveTVChainsLock);

    vector<LiveTVChain*>::iterator it = liveTVChains.begin();
    for (; it != liveTVChains.end(); ++it)
    {
        if ((*it)->ProgramIsAt(pginfo) >= 0)
            return *it;
    }

    return NULL;
}

void MainServer::AddToChains(LiveTVChain *chain)
{
    QMutexLocker lock(&liveTVChainsLock);

    if (chain)
        liveTVChains.push_back(chain);
}

void MainServer::DeleteChain(LiveTVChain *chain)
{
    QMutexLocker lock(&liveTVChainsLock);

    if (!chain)
        return;

    vector<LiveTVChain*> newChains;

    vector<LiveTVChain*>::iterator it = liveTVChains.begin();
    for (; it != liveTVChains.end(); ++it)
    {
        if (*it != chain)
            newChains.push_back(*it);
    }
    liveTVChains = newChains;

    delete chain;
}

void MainServer::SetExitCode(int exitCode, bool closeApplication)
{
    m_exitCode = exitCode;
    if (closeApplication)
        QCoreApplication::exit(m_exitCode);
}

QString MainServer::LocalFilePath(const QUrl &url, const QString wantgroup)
{
    QString lpath = url.path();

    if (lpath.section('/', -2, -2) == "channels")
    {
        // This must be an icon request. Check channel.icon to be safe.
        QString querytext;

        QString file = lpath.section('/', -1);
        lpath = "";

        MSqlQuery query(MSqlQuery::InitCon());
        query.prepare("SELECT icon FROM channel WHERE icon LIKE :FILENAME ;");
        query.bindValue(":FILENAME", QString("%") + file);

        if (query.exec() && query.isActive() && query.size())
        {
            query.next();
            lpath = query.value(0).toString();
        }
        else
        {
            MythDB::DBError("Icon path", query);
        }
    }
    else
    {
        lpath = lpath.section('/', -1);

        QString fpath = lpath;
        if (fpath.right(4) == ".png")
            fpath = fpath.left(fpath.length() - 4);

        ProgramInfo *pginfo = ProgramInfo::GetProgramFromBasename(fpath);
        if (pginfo)
        {
            QString pburl = GetPlaybackURL(pginfo);
            if (pburl.left(1) == "/")
            {
                lpath = pburl.section('/', 0, -2) + "/" + lpath;
                VERBOSE(VB_FILE, QString("Local file path: %1").arg(lpath));
            }
            else
            {
                VERBOSE(VB_IMPORTANT,
                        QString("ERROR: LocalFilePath unable to find local "
                                "path for '%1', found '%2' instead.")
                                .arg(lpath).arg(pburl));
                lpath = "";
            }

            delete pginfo;
        }
        else if (!lpath.isEmpty())
        {
            // For securities sake, make sure filename is really the pathless.
            QString opath = lpath;
            StorageGroup sgroup;

            if (!wantgroup.isEmpty())
            {
                sgroup.Init(wantgroup);
                lpath = url.toString();
            }
            else
            {
                lpath = QFileInfo(lpath).fileName();
            }

            QString tmpFile = sgroup.FindRecordingFile(lpath);
            if (!tmpFile.isEmpty())
            {
                lpath = tmpFile;
                VERBOSE(VB_FILE,
                        QString("LocalFilePath(%1 '%2')")
                        .arg(url.toString()).arg(opath)
                        <<", found file through exhaustive search "
                        <<QString("at '%1'").arg(lpath));
            }
            else
            {
                VERBOSE(VB_IMPORTANT, "ERROR: LocalFilePath "
                        <<QString("unable to find local path for '%1'.")
                        .arg(opath));
                lpath = "";
            }

        }
        else
        {
            lpath = "";
        }
    }

    return lpath;
}

void MainServer::reconnectTimeout(void)
{
    MythSocket *masterServerSock = new MythSocket();

    QString server = gContext->GetSetting("MasterServerIP", "127.0.0.1");
    int port = gContext->GetNumSetting("MasterServerPort", 6543);

    VERBOSE(VB_IMPORTANT, QString("Connecting to master server: %1:%2")
                           .arg(server).arg(port));

    if (!masterServerSock->connect(server, port))
    {
        VERBOSE(VB_IMPORTANT, "Connection to master server timed out.");
        masterServerReconnect->start(kMasterServerReconnectTimeout);
        masterServerSock->DownRef();
        return;
    }

    if (masterServerSock->state() != MythSocket::Connected)
    {
        VERBOSE(VB_IMPORTANT, "Could not connect to master server.");
        masterServerReconnect->start(kMasterServerReconnectTimeout);
        masterServerSock->DownRef();
        return;
    }

    VERBOSE(VB_IMPORTANT, "Connected successfully");

    QString str = QString("ANN SlaveBackend %1 %2")
                          .arg(gContext->GetHostName())
                          .arg(gContext->GetSetting("BackendServerIP"));

    masterServerSock->Lock();

    QStringList strlist( str );

    QMap<int, EncoderLink *>::Iterator iter = encoderList->begin();
    for (; iter != encoderList->end(); ++iter)
    {
        EncoderLink *elink = *iter;
        elink->CancelNextRecording(true);
        ProgramInfo *pinfo = elink->GetRecording();
        if (pinfo)
        {
            pinfo->ToStringList(strlist);
            delete pinfo;
        }
        else
        {
            ProgramInfo dummy;
            dummy.ToStringList(strlist);
        }
    }

    if (!masterServerSock->writeStringList(strlist) ||
        !masterServerSock->readStringList(strlist) ||
        strlist.empty() || strlist[0] == "ERROR")
    {
        masterServerSock->DownRef();
        masterServerSock->Unlock();
        masterServerSock = NULL;
        if (strlist.empty())
        {
            VERBOSE(VB_IMPORTANT, LOC_ERR +
                    "Failed to open master server socket, timeout");
        }
        else
        {
            VERBOSE(VB_IMPORTANT, LOC_ERR +
                    "Failed to open master server socket" +
                    ((strlist.size() >= 2) ?
                     QString(", error was %1").arg(strlist[1]) :
                     QString(", remote error")));
        }
        masterServerReconnect->start(kMasterServerReconnectTimeout);
        return;
    }

    masterServerSock->setCallbacks(this);

    masterServer = new PlaybackSock(this, masterServerSock, server,
                                    kPBSEvents_Normal);
    sockListLock.lockForWrite();
    playbackList.push_back(masterServer);
    sockListLock.unlock();

    masterServerSock->Unlock();

    autoexpireUpdateTimer->start(1000);
}

// returns true, if a client (slavebackends are not counted!)
// is connected by checking the lists.
bool MainServer::isClientConnected()
{
    bool foundClient = false;

    sockListLock.lockForRead();

    foundClient |= !fileTransferList.empty();

    vector<PlaybackSock *>::iterator it = playbackList.begin();
    for (; !foundClient && (it != playbackList.end()); ++it)
    {
        // we simply ignore slaveBackends!
        // and clients that don't want to block shutdown
        if (!(*it)->isSlaveBackend() && (*it)->getBlockShutdown())
            foundClient = true;
    }

    sockListLock.unlock();

    return (foundClient);
}

/// Sends the Slavebackends the request to shut down using haltcmd
void MainServer::ShutSlaveBackendsDown(QString &haltcmd)
{
// TODO FIXME We should issue a MythEvent and have customEvent
// send this with the proper syncronization and locking.

    QStringList bcast( "SHUTDOWN_NOW" );
    bcast << haltcmd;

    sockListLock.lockForRead();

    vector<PlaybackSock *>::iterator it = playbackList.begin();
    for (; it != playbackList.end(); ++it)
    {
        if ((*it)->isSlaveBackend())
            (*it)->getSocket()->writeStringList(bcast);
    }

    sockListLock.unlock();
}

/* vim: set expandtab tabstop=4 shiftwidth=4: */
