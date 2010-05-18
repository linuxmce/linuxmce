// -*- Mode: c++ -*-
// vim:set sw=4 ts=4 expandtab:
// Copyright (c) 2009, Daniel Thor Kristjansson
// Distributed as part of MythTV under GPL version 2
// (or at your option a later version)

#include <QCoreApplication>
#include <QThreadPool>

#include "programinfocache.h"
#include "programinfo.h"
#include "remoteutil.h"
#include "mythevent.h"

typedef vector<ProgramInfo*> *VPI_ptr;
static void free_vec(VPI_ptr &v)
{
    if (v)
    {
        vector<ProgramInfo*>::iterator it = v->begin();
        for (; it != v->end(); ++it)
            delete *it;
        delete v;
        v = NULL;
    }
}

class ProgramInfoLoader : public QRunnable
{
  public:
    ProgramInfoLoader(ProgramInfoCache &c) : m_cache(c) {}

    void run(void) { m_cache.Load(); }

    ProgramInfoCache &m_cache;
};

ProgramInfoCache::ProgramInfoCache(QObject *o) :
    m_next_cache(NULL), m_listener(o),
    m_load_is_queued(false), m_loads_in_progress(0)
{
}

ProgramInfoCache::~ProgramInfoCache()
{
    QMutexLocker locker(&m_lock);

    while (m_loads_in_progress)
        m_load_wait.wait(&m_lock);

    Clear();
    free_vec(m_next_cache);
}

void ProgramInfoCache::ScheduleLoad(void)
{
    QMutexLocker locker(&m_lock);
    if (!m_load_is_queued)
    {
        m_load_is_queued = true;
        m_loads_in_progress++;
        QThreadPool::globalInstance()->start(
            new ProgramInfoLoader(*this));
    }
}

void ProgramInfoCache::Load(void)
{
    QMutexLocker locker(&m_lock);
    m_load_is_queued = false;

    locker.unlock();
    /**/
    // the param to RemoteGetRecordedList doesn't actually matter
    // we sort the list later anyway.
    vector<ProgramInfo*> *tmp = RemoteGetRecordedList(false);
    /**/
    locker.relock();

    free_vec(m_next_cache);
    m_next_cache = tmp;

    QCoreApplication::postEvent(
        m_listener, new MythEvent("UPDATE_UI_LIST"));
    
    m_loads_in_progress--;
    m_load_wait.wakeAll();
}

bool ProgramInfoCache::IsLoadInProgress(void) const
{
    QMutexLocker locker(&m_lock);
    return m_loads_in_progress;
}

void ProgramInfoCache::WaitForLoadToComplete(void) const
{
    QMutexLocker locker(&m_lock);
    while (m_loads_in_progress)
        m_load_wait.wait(&m_lock);
}

/** \brief Refreshed the cache.
 *  
 *  If a new list has been loaded this fills the cache with that list
 *  if not, this simply removes list items marked for deletion from the
 *  the list.
 *
 *  \note This must only be called from the UI thread.
 *  \note All references to the ProgramInfo pointers should be cleared
 *        before this is called.
 */
void ProgramInfoCache::Refresh(void)
{
    QMutexLocker locker(&m_lock);
    if (m_next_cache)
    {
        Clear();
        vector<ProgramInfo*>::iterator it = m_next_cache->begin();
        for (; it != m_next_cache->end(); ++it)
        {
            PICKey k((*it)->chanid.toUInt(), (*it)->recstartts);
            m_cache[k] = *it;
        }
        delete m_next_cache;
        m_next_cache = NULL;
        return;
    }
    locker.unlock();

    Cache::iterator it = m_cache.begin();
    Cache::iterator nit = it;
    for (; it != m_cache.end(); it = nit)
    {
        nit = it;
        nit++;

        if (it->second->availableStatus == asDeleted)
        {
            delete it->second;
            m_cache.erase(it);
        }
    }
}

/** \brief Updates a ProgramInfo in the cache.
 *  \note This must only be called from the UI thread.
 *  \return True iff the ProgramInfo was in the cache and was updated.
 */
bool ProgramInfoCache::Update(const ProgramInfo &pginfo)
{
    QMutexLocker locker(&m_lock);

    Cache::iterator it = m_cache.find(
        PICKey(pginfo.chanid.toUInt(),pginfo.recstartts));

    if (it != m_cache.end())
    {
        QString pathname = it->second->pathname;
        *(it->second) = pginfo;
        it->second->pathname = pathname;
    }

    return it != m_cache.end();
}

/** \brief Updates a ProgramInfo in the cache.
 *  \note This must only be called from the UI thread.
 *  \return True iff the ProgramInfo was in the cache and was updated.
 */
bool ProgramInfoCache::UpdateFileSize(
    uint chanid, const QDateTime &recstartts, uint64_t filesize)
{
    QMutexLocker locker(&m_lock);

    Cache::iterator it = m_cache.find(PICKey(chanid,recstartts));

    if (it != m_cache.end())
    {
        it->second->filesize = filesize;
        if (filesize)
            it->second->availableStatus = asAvailable;
    }

    return it != m_cache.end();
}

/** \brief Returns the ProgramInfo::recgroup or an empty string if not found.
 *  \note This must only be called from the UI thread.
 */
QString ProgramInfoCache::GetRecGroup(
    uint chanid, const QDateTime &recstartts) const
{
    QMutexLocker locker(&m_lock);

    Cache::const_iterator it = m_cache.find(PICKey(chanid,recstartts));

    QString recgroup;
    if (it != m_cache.end())
        recgroup = it->second->recgroup;

    return recgroup;
}

/** \brief Adds a ProgramInfo to the cache.
 *  \note This must only be called from the UI thread.
 */
void ProgramInfoCache::Add(const ProgramInfo &pginfo)
{
    if (Update(pginfo))
        return;

    PICKey key(pginfo.chanid.toUInt(),pginfo.recstartts);
    m_cache[key] = new ProgramInfo(pginfo);
}

/** \brief Marks a ProgramInfo in the cache for deletion on the next
 *         call to Refresh().
 *  \note This must only be called from the UI thread.
 *  \return True iff the ProgramInfo was in the cache.
 */
bool ProgramInfoCache::Remove(uint chanid, const QDateTime &recstartts)
{
    Cache::iterator it = m_cache.find(PICKey(chanid,recstartts));

    if (it != m_cache.end())
        it->second->availableStatus = asDeleted;

    return it != m_cache.end();
}

void ProgramInfoCache::GetOrdered(vector<ProgramInfo*> &list, bool newest_first)
{
    if (newest_first)
    {
        Cache::reverse_iterator it = m_cache.rbegin();
        for (; it != m_cache.rend(); ++it)
            list.push_back(it->second);
    }
    else
    {
        for (Cache::iterator it = m_cache.begin(); it != m_cache.end(); ++it)
            list.push_back(it->second);
    }
}

ProgramInfo *ProgramInfoCache::GetProgramInfo(
    uint chanid, const QDateTime &recstartts) const
{
    Cache::const_iterator it = m_cache.find(PICKey(chanid,recstartts));
    
    if (it != m_cache.end())
        return it->second;

    return NULL;
}

ProgramInfo *ProgramInfoCache::GetProgramInfo(const QString &piKey) const
{
    uint      chanid;
    QDateTime recstartts;
    if (ProgramInfo::ExtractKey(piKey, chanid, recstartts))
        return GetProgramInfo(chanid, recstartts);
    return NULL;
}

/// Clears the cache, m_lock must be held when this is called.
void ProgramInfoCache::Clear(void)
{
    for (Cache::iterator it = m_cache.begin(); it != m_cache.end(); ++it)
        delete it->second;
    m_cache.clear();
}

