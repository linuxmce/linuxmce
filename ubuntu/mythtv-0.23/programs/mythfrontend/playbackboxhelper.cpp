#include <algorithm>
using namespace std;

#include <QCoreApplication>
#include <QStringList>
#include <QDateTime>
#include <QFileInfo>

#include "playbackboxhelper.h"
#include "previewgenerator.h"
#include "tvremoteutil.h"
#include "programinfo.h"
#include "mythcontext.h"
#include "remoteutil.h"
#include "mythevent.h"
#include "mythdirs.h"

#define LOC      QString("PlaybackBoxHelper: ")
#define LOC_WARN QString("PlaybackBoxHelper Warning: ")
#define LOC_ERR  QString("PlaybackBoxHelper Error: ")

class PBHEventHandler : public QObject
{
  public:
    PBHEventHandler(PlaybackBoxHelper &pbh) :
        m_pbh(pbh), m_freeSpaceTimerId(0) { }
    virtual bool event(QEvent*); // QObject
    void UpdateFreeSpaceEvent(void);
    PlaybackBoxHelper &m_pbh;
    int m_freeSpaceTimerId;
    static const uint kUpdateFreeSpaceInterval;
};

const uint PBHEventHandler::kUpdateFreeSpaceInterval = 15000; // 15 seconds

bool PBHEventHandler::event(QEvent *e)
{
    if (e->type() == QEvent::Timer)
    {
        QTimerEvent *te = (QTimerEvent*)e;
        const int timer_id = te->timerId();
        if (timer_id == m_freeSpaceTimerId)
            UpdateFreeSpaceEvent();
    }
    else if (e->type() == (QEvent::Type) MythEvent::MythEventMessage)
    {
        MythEvent *me = (MythEvent*)e;
        if (me->Message() == "UPDATE_FREE_SPACE")
        {
            UpdateFreeSpaceEvent();
            return true;
        }
        else if (me->Message() == "STOP_RECORDING")
        {
            ProgramInfo pginfo;
            if (pginfo.FromStringList(me->ExtraDataList(), 0))
                RemoteStopRecording(&pginfo);
            return true;
        }
        else if (me->Message() == "DELETE_RECORDINGS")
        {
            QStringList successes;
            QStringList failures;
            QStringList list = me->ExtraDataList();
            while (list.size() >= 3)
            {
                uint      chanid        = list[0].toUInt();
                QDateTime recstartts    = QDateTime::fromString(
                    list[1], Qt::ISODate);
                bool      forceDelete   = list[2].toUInt();

                bool ok = RemoteDeleteRecording(
                    chanid, recstartts, forceDelete);

                QStringList &res = (ok) ? successes : failures;
                for (uint i = 0; i < 3; i++)
                {
                    res.push_back(list.front());
                    list.pop_front();
                }
            }
            if (!successes.empty())
            {
                MythEvent *e = new MythEvent("DELETE_SUCCESSES", successes);
                QCoreApplication::postEvent(m_pbh.m_listener, e);
            }
            if (!failures.empty())
            {
                MythEvent *e = new MythEvent("DELETE_FAILURES", failures);
                QCoreApplication::postEvent(m_pbh.m_listener, e);
            }
        }
        else if (me->Message() == "UNDELETE_RECORDINGS")
        {
            QStringList successes;
            QStringList failures;
            QStringList list = me->ExtraDataList();
            while (list.size() >= 2)
            {
                uint      chanid        = list[0].toUInt();
                QDateTime recstartts    = QDateTime::fromString(
                    list[1], Qt::ISODate);

                bool ok = RemoteUndeleteRecording(chanid, recstartts);

                QStringList &res = (ok) ? successes : failures;
                for (uint i = 0; i < 2; i++)
                {
                    res.push_back(list.front());
                    list.pop_front();
                }
            }
            if (!successes.empty())
            {
                MythEvent *e = new MythEvent("UNDELETE_SUCCESSES", successes);
                QCoreApplication::postEvent(m_pbh.m_listener, e);
            }
            if (!failures.empty())
            {
                MythEvent *e = new MythEvent("UNDELETE_FAILURES", failures);
                QCoreApplication::postEvent(m_pbh.m_listener, e);
            }
        }
        else if (me->Message() == "GET_PREVIEW")
        {
            ProgramInfo evinfo;
            if (evinfo.FromStringList(me->ExtraDataList(), 0) &&
                evinfo.IsFileReadable())
            {
                // Note: IsFileReadable() implicitly calls GetPlaybackURL()
                // when necessary, so we might as well update the pathname
                // in the PBB cache to avoid calling this again...
                QStringList list;
                list.push_back(evinfo.MakeUniqueKey());
                list.push_back(evinfo.pathname);            
                MythEvent *e0 = new MythEvent("SET_PLAYBACK_URL", list);
                QCoreApplication::postEvent(m_pbh.m_listener, e0);

                QString fn = m_pbh.GeneratePreviewImage(evinfo);
                if (!fn.isEmpty())
                {
                    QStringList list;
                    list.push_back(evinfo.MakeUniqueKey());
                    list.push_back(fn);
                    MythEvent *e = new MythEvent("PREVIEW_READY", list);
                    QCoreApplication::postEvent(m_pbh.m_listener, e);
                }
            }
        }
        else if (me->Message() == "CHECK_AVAILABILITY")
        {
            CheckAvailabilityType cat = kCheckForCache;
            QTime tm;
            if (me->ExtraDataCount() >= 5)
            {
                cat = (CheckAvailabilityType) me->ExtraData(0).toUInt();
                tm.setHMS(
                    me->ExtraData(1).toUInt(),
                    me->ExtraData(2).toUInt(),
                    me->ExtraData(3).toUInt(),
                    me->ExtraData(4).toUInt());
            }
            ProgramInfo evinfo;
            if (!evinfo.FromStringList(me->ExtraDataList(), 5))
                return true;

            AvailableStatusType availableStatus = asAvailable;
            // Note IsFileReadable() implicitly calls GetPlaybackURL
            // when necessary, we rely on this.
            if (!evinfo.IsFileReadable())
            {
                VERBOSE(VB_IMPORTANT, LOC_ERR +
                        QString("CHECK_AVAILABILITY '%1' "
                                "file not found")
                        .arg(evinfo.pathname));
                availableStatus = asFileNotFound;
            }
            else if (0 == evinfo.filesize)
            {
                evinfo.filesize = evinfo.GetFilesize();
                if (0 == evinfo.filesize)
                {
                    availableStatus = (evinfo.recstatus == rsRecording) ?
                        asNotYetAvailable : asZeroByte;
                }
            }

            QStringList list;
            list.push_back(evinfo.MakeUniqueKey());
            list.push_back(evinfo.pathname);            
            MythEvent *e0 = new MythEvent("SET_PLAYBACK_URL", list);
            QCoreApplication::postEvent(m_pbh.m_listener, e0);

            list.clear();
            list.push_back(evinfo.MakeUniqueKey());
            list.push_back(QString::number((int)cat));
            list.push_back(QString::number((int)availableStatus));
            list.push_back(QString::number(evinfo.filesize));
            list.push_back(QString::number(tm.hour()));
            list.push_back(QString::number(tm.minute()));
            list.push_back(QString::number(tm.second()));
            list.push_back(QString::number(tm.msec()));
            MythEvent *e = new MythEvent("AVAILABILITY", list);
            QCoreApplication::postEvent(m_pbh.m_listener, e);
        }
    }

    return QObject::event(e);
}

void PBHEventHandler::UpdateFreeSpaceEvent(void)
{
    if (m_freeSpaceTimerId)
        killTimer(m_freeSpaceTimerId);
    m_pbh.UpdateFreeSpace();
    m_freeSpaceTimerId = startTimer(kUpdateFreeSpaceInterval);
}

//////////////////////////////////////////////////////////////////////

const uint PreviewGenState::maxAttempts     = 5;
const uint PreviewGenState::minBlockSeconds = 60;

PlaybackBoxHelper::PlaybackBoxHelper(QObject *listener) :
    m_listener(listener), m_eventHandler(NULL),
    // Free Space Tracking Variables
    m_freeSpaceTotalMB(0ULL), m_freeSpaceUsedMB(0ULL),
    // Preview Image Variables
    m_previewGeneratorRunning(0), m_previewGeneratorMaxThreads(2)
{
    m_previewGeneratorMode =
        gContext->GetNumSetting("GeneratePreviewRemotely", 0) ?
        PreviewGenerator::kRemote : PreviewGenerator::kLocalAndRemote;

    int idealThreads = QThread::idealThreadCount();
    if (idealThreads >= 1)
        m_previewGeneratorMaxThreads = idealThreads * 2;

    start();
}

PlaybackBoxHelper::~PlaybackBoxHelper()
{
    exit();
    wait();

    // disconnect preview generators
    QMutexLocker locker(&m_previewGeneratorLock);
    PreviewMap::iterator it = m_previewGenerator.begin();
    for (;it != m_previewGenerator.end(); ++it)
    {
        if ((*it).gen)
            (*it).gen->disconnectSafe();
    }

    // delete the event handler
    delete m_eventHandler;
    m_eventHandler = NULL;
}

void PlaybackBoxHelper::ForceFreeSpaceUpdate(void)
{
    QCoreApplication::postEvent(
        m_eventHandler, new MythEvent("UPDATE_FREE_SPACE"));
}

void PlaybackBoxHelper::StopRecording(const ProgramInfo &pginfo)
{
    QStringList list;
    pginfo.ToStringList(list);
    MythEvent *e = new MythEvent("STOP_RECORDING", list);
    QCoreApplication::postEvent(m_eventHandler, e);
}

void PlaybackBoxHelper::DeleteRecording(
    uint chanid, const QDateTime &recstartts, bool forceDelete)
{
    QStringList list;
    list.push_back(QString::number(chanid));
    list.push_back(recstartts.toString(Qt::ISODate));
    list.push_back((forceDelete)    ? "1" : "0");
    DeleteRecordings(list);
}

void PlaybackBoxHelper::DeleteRecordings(const QStringList &list)
{
    MythEvent *e = new MythEvent("DELETE_RECORDINGS", list);
    QCoreApplication::postEvent(m_eventHandler, e);
}

void PlaybackBoxHelper::UndeleteRecording(
    uint chanid, const QDateTime &recstartts)
{
    QStringList list;
    list.push_back(QString::number(chanid));
    list.push_back(recstartts.toString(Qt::ISODate));
    MythEvent *e = new MythEvent("UNDELETE_RECORDINGS", list);
    QCoreApplication::postEvent(m_eventHandler, e);
}

void PlaybackBoxHelper::run(void)
{
    m_eventHandler = new PBHEventHandler(*this);
    exec();
}

void PlaybackBoxHelper::UpdateFreeSpace(void)
{
    vector<FileSystemInfo> fsInfos = RemoteGetFreeSpace();

    QMutexLocker locker(&m_lock);
    for (uint i = 0; i < fsInfos.size(); i++)
    {
        if (fsInfos[i].directory == "TotalDiskSpace")
        {
            m_freeSpaceTotalMB = (uint64_t) (fsInfos[i].totalSpaceKB >> 10);
            m_freeSpaceUsedMB  = (uint64_t) (fsInfos[i].usedSpaceKB  >> 10);
        }
    }
}

uint64_t PlaybackBoxHelper::GetFreeSpaceTotalMB(void) const
{
    QMutexLocker locker(&m_lock);
    return m_freeSpaceTotalMB;
}

uint64_t PlaybackBoxHelper::GetFreeSpaceUsedMB(void) const
{
    QMutexLocker locker(&m_lock);
    return m_freeSpaceUsedMB;
}

void PlaybackBoxHelper::CheckAvailability(
    const ProgramInfo &pginfo, CheckAvailabilityType cat)
{
    QTime tm = QTime::currentTime();
    QStringList list(QString::number((int)cat));
    list.push_back(QString::number(tm.hour()));
    list.push_back(QString::number(tm.minute()));
    list.push_back(QString::number(tm.second()));
    list.push_back(QString::number(tm.msec()));
    pginfo.ToStringList(list);
    MythEvent *e = new MythEvent("CHECK_AVAILABILITY", list);
    QCoreApplication::postEvent(m_eventHandler, e);        
}

void PlaybackBoxHelper::GetPreviewImage(const ProgramInfo &pginfo)
{
    QStringList extra;
    pginfo.ToStringList(extra);
    MythEvent *e = new MythEvent("GET_PREVIEW", extra);
    QCoreApplication::postEvent(m_eventHandler, e);        
}

QString PlaybackBoxHelper::GeneratePreviewImage(ProgramInfo &pginfo)
{
    if (pginfo.availableStatus == asPendingDelete)
        return QString();

    QString filename = pginfo.pathname + ".png";

    // If someone is asking for this preview it must be on screen
    // and hence higher priority than anything else we may have
    // queued up recently....
    IncPreviewGeneratorPriority(filename);

    QDateTime previewLastModified;
    QString ret_file = filename;
    bool streaming = filename.left(1) != "/";
    bool locally_accessible = false;
    bool bookmark_updated = false;

    QDateTime bookmark_ts = pginfo.GetBookmarkTimeStamp();
    QDateTime cmp_ts = bookmark_ts.isValid() ?
        bookmark_ts : pginfo.lastmodified;

    if (streaming)
    {
        ret_file = QString("%1/remotecache/%2")
            .arg(GetConfDir()).arg(filename.section('/', -1));

        QFileInfo finfo(ret_file);
        if (finfo.isReadable() && finfo.lastModified() >= cmp_ts)
        {
            // This is just an optimization to avoid
            // hitting the backend if our cached copy
            // is newer than the bookmark, or if we have
            // a preview and do not update it when the
            // bookmark changes.
            previewLastModified = finfo.lastModified();
        }
        else
        {
            previewLastModified =
                RemoteGetPreviewIfModified(pginfo, ret_file);
        }
    }
    else
    {
        QFileInfo fi(filename);
        if ((locally_accessible = fi.isReadable()))
            previewLastModified = fi.lastModified();
    }

    bookmark_updated =
        (!previewLastModified.isValid() || (previewLastModified < cmp_ts));

    if (bookmark_updated && bookmark_ts.isValid() &&
        previewLastModified.isValid())
    {
        ClearPreviewGeneratorAttempts(filename);
    }

    if (0)
    {
        VERBOSE(VB_IMPORTANT, QString(
                    "previewLastModified:  %1\n\t\t\t"
                    "bookmark_ts:          %2\n\t\t\t"
                    "pginfo.lastmodified: %3")
                .arg(previewLastModified.toString(Qt::ISODate))
                .arg(bookmark_ts.toString(Qt::ISODate))
                .arg(pginfo.lastmodified.toString(Qt::ISODate)));
    }

    bool preview_exists = previewLastModified.isValid();

    if (0)
    {
        VERBOSE(VB_IMPORTANT,
                QString("Title: %1:%2\n\t\t\t")
                .arg(pginfo.title).arg(pginfo.subtitle) +
                QString("File  '%1' \n\t\t\tCache '%2'")
                .arg(filename).arg(ret_file) +
                QString("\n\t\t\tPreview Exists: %1, "
                        "Bookmark Updated: %2, "
                        "Need Preview: %3")
                .arg(preview_exists).arg(bookmark_updated)
                .arg((bookmark_updated || !preview_exists)));
    }

    if ((bookmark_updated || !preview_exists) &&
        !IsGeneratingPreview(filename))
    {
        uint attempts = IncPreviewGeneratorAttempts(filename);
        if (attempts < PreviewGenState::maxAttempts)
        {
            VERBOSE(VB_PLAYBACK, LOC +
                    QString("Requesting preview for '%1'")
                    .arg(filename));
            PreviewGenerator::Mode mode =
                (PreviewGenerator::Mode) m_previewGeneratorMode;
            PreviewGenerator *pg = new PreviewGenerator(&pginfo, mode);
            while (!SetPreviewGenerator(filename, pg)) usleep(50000);
            VERBOSE(VB_PLAYBACK, LOC +
                    QString("Requested preview for '%1'")
                    .arg(filename));
        }
        else if (attempts == PreviewGenState::maxAttempts)
        {
            VERBOSE(VB_IMPORTANT, LOC_ERR +
                    QString("Attempted to generate preview for '%1' "
                            "%2 times, giving up.")
                    .arg(filename).arg(PreviewGenState::maxAttempts));
        }
    }
    else if (bookmark_updated || !preview_exists)
    {
        VERBOSE(VB_PLAYBACK, LOC +
                "Not requesting preview as it "
                "is already being generated");
    }

    UpdatePreviewGeneratorThreads();

    QString ret = (locally_accessible) ?
        filename : (previewLastModified.isValid()) ?
        ret_file : (QFileInfo(ret_file).isReadable()) ?
        ret_file : QString();

    //VERBOSE(VB_IMPORTANT, QString("Returning: '%1'").arg(ret));

    return ret;
}

void PlaybackBoxHelper::IncPreviewGeneratorPriority(const QString &xfn)
{
    QString fn = xfn.mid(max(xfn.lastIndexOf('/') + 1,0));

    QMutexLocker locker(&m_previewGeneratorLock);
    m_previewGeneratorQueue.removeAll(fn);

    PreviewMap::iterator pit = m_previewGenerator.find(fn);
    if (pit != m_previewGenerator.end() && (*pit).gen && !(*pit).genStarted)
        m_previewGeneratorQueue.push_back(fn);
}

void PlaybackBoxHelper::UpdatePreviewGeneratorThreads(void)
{
    QMutexLocker locker(&m_previewGeneratorLock);
    QStringList &q = m_previewGeneratorQueue;
    if (!q.empty() && 
        (m_previewGeneratorRunning < m_previewGeneratorMaxThreads))
    {
        QString fn = q.back();
        q.pop_back();
        PreviewMap::iterator it = m_previewGenerator.find(fn);
        if (it != m_previewGenerator.end() && (*it).gen && !(*it).genStarted)
        {
            m_previewGeneratorRunning++;
            (*it).gen->Start();
            (*it).genStarted = true;
        }
    }
}

/** \fn PlaybackBoxHelper::SetPreviewGenerator(const QString&, PreviewGenerator*)
 *  \brief Sets the PreviewGenerator for a specific file.
 *  \return true iff call succeeded.
 */
bool PlaybackBoxHelper::SetPreviewGenerator(const QString &xfn, PreviewGenerator *g)
{
    QString fn = xfn.mid(max(xfn.lastIndexOf('/') + 1,0));

    if (!m_previewGeneratorLock.tryLock())
        return false;

    if (!g)
    {
        m_previewGeneratorRunning = max(0, (int)m_previewGeneratorRunning - 1);
        PreviewMap::iterator it = m_previewGenerator.find(fn);
        if (it == m_previewGenerator.end())
        {
            m_previewGeneratorLock.unlock();
            return false;
        }

        (*it).gen        = NULL;
        (*it).genStarted = false;
        (*it).ready      = false;
        (*it).lastBlockTime =
            max(PreviewGenState::minBlockSeconds, (*it).lastBlockTime * 2);
        (*it).blockRetryUntil =
            QDateTime::currentDateTime().addSecs((*it).lastBlockTime);

        m_previewGeneratorLock.unlock();
        return true;
    }

    g->AttachSignals(this);
    m_previewGenerator[fn].gen = g;
    m_previewGenerator[fn].genStarted = false;
    m_previewGenerator[fn].ready = false;

    m_previewGeneratorLock.unlock();
    IncPreviewGeneratorPriority(xfn);

    return true;
}

/** \fn PlaybackBoxHelper::IsGeneratingPreview(const QString&, bool) const
 *  \brief Returns true if we have already started a
 *         PreviewGenerator to create this file.
 */
bool PlaybackBoxHelper::IsGeneratingPreview(const QString &xfn, bool really) const
{
    PreviewMap::const_iterator it;
    QMutexLocker locker(&m_previewGeneratorLock);

    QString fn = xfn.mid(max(xfn.lastIndexOf('/') + 1,0));
    if ((it = m_previewGenerator.find(fn)) == m_previewGenerator.end())
        return false;

    if (really)
        return ((*it).gen && !(*it).ready);

    if ((*it).blockRetryUntil.isValid())
        return QDateTime::currentDateTime() < (*it).blockRetryUntil;

    return (*it).gen;
}

/** \fn PlaybackBoxHelper::IncPreviewGeneratorAttempts(const QString&)
 *  \brief Increments and returns number of times we have
 *         started a PreviewGenerator to create this file.
 */
uint PlaybackBoxHelper::IncPreviewGeneratorAttempts(const QString &xfn)
{
    QMutexLocker locker(&m_previewGeneratorLock);
    QString fn = xfn.mid(max(xfn.lastIndexOf('/') + 1,0));
    return m_previewGenerator[fn].attempts++;
}

/** \fn PlaybackBoxHelper::ClearPreviewGeneratorAttempts(const QString&)
 *  \brief Clears the number of times we have
 *         started a PreviewGenerator to create this file.
 */
void PlaybackBoxHelper::ClearPreviewGeneratorAttempts(const QString &xfn)
{
    QMutexLocker locker(&m_previewGeneratorLock);
    QString fn = xfn.mid(max(xfn.lastIndexOf('/') + 1,0));
    m_previewGenerator[fn].attempts = 0;
    m_previewGenerator[fn].lastBlockTime = 0;
    m_previewGenerator[fn].blockRetryUntil =
        QDateTime::currentDateTime().addSecs(-60);
}

void PlaybackBoxHelper::previewThreadDone(const QString &fn, bool &success)
{
    VERBOSE(VB_PLAYBACK, LOC + QString("Preview for '%1' done").arg(fn));
    success = SetPreviewGenerator(fn, NULL);
    UpdatePreviewGeneratorThreads();
}

/** \fn PlaybackBoxHelper::previewReady(const ProgramInfo*)
 *  \brief Callback used by PreviewGenerator to tell us a m_preview
 *         we requested has been returned from the backend.
 *  \param pginfo ProgramInfo describing the previewed recording.
 */
void PlaybackBoxHelper::previewReady(const ProgramInfo *pginfo)
{
    if (!pginfo)
        return;
    
    QString xfn = pginfo->pathname + ".png";
    QString fn = xfn.mid(max(xfn.lastIndexOf('/') + 1,0));

    VERBOSE(VB_PLAYBACK, LOC + QString("Preview for '%1' ready")
            .arg(pginfo->pathname));

    m_previewGeneratorLock.lock();
    PreviewMap::iterator it = m_previewGenerator.find(fn);
    if (it != m_previewGenerator.end())
    {
        (*it).ready         = true;
        (*it).attempts      = 0;
        (*it).lastBlockTime = 0;
    }
    m_previewGeneratorLock.unlock();

    if (pginfo)
    {
        QStringList list;
        list.push_back(pginfo->MakeUniqueKey());
        list.push_back(xfn);
        MythEvent *e = new MythEvent("PREVIEW_READY", list);
        QCoreApplication::postEvent(m_listener, e);        
    }
}
