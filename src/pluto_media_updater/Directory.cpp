#include <iostream>

#include "Directory.h"

#include <attr/attributes.h>
#include <glib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>


Directory::Directory(Controller& controller, const string& name, Directory* parent)
    : m_controller(controller), m_dir(NULL), m_basePath(""), m_name(""), m_parent(parent)
{
    char *path = g_path_get_dirname(name.c_str());
    if (path)
    {
        if (path[0] != '.')
            m_basePath = path;
        free(path);
    }
    if (basename(name.c_str()))
        m_name = string( basename(name.c_str()) );
    m_dir = opendir(getPath().c_str());
    if (m_dir)
    {
        fcntl(dirfd(m_dir), F_SETSIG, SIGRTMIN+1);
        fcntl(dirfd(m_dir), F_NOTIFY, DN_CREATE|DN_DELETE|DN_RENAME|DN_MULTISHOT);
    }
    else
        throw(string(strerror(errno)) + ": " + getPath());

    m_pFiles = new FileByIdMap();
    m_pFilesTemp = new FileByIdMap();

    m_pNewFiles = new FileByNameMap();
    m_pNewFilesTemp = new FileByNameMap();

    scan();
}

Directory::~Directory()
{
    removeChildren();
    closedir(m_dir);
}

string Directory::getPath() const
{
    if (m_parent)
        return m_parent->getPath() + "/" + this->getName();
    else
        return this->m_basePath + "/" + this->getName();
}

void Directory::checkFile(const string& name)
{
    string fullname = this->getPath() + "/" + name;
    int n = 79;
    char value[80];
    uint64_t fileKey = 0;

    memset(value, 0, sizeof(value));

    if  ( attr_get(fullname.c_str(), "ID", value, &n, 0) == 0 )
        fileKey = atoll(value);

    File* f = new File(fileKey, name, this);
    if ( f->key == 0 )
    {
        cout << "New file: " << f->name << endl;
        m_pNewFilesTemp->insert( make_pair(f->name, f) );

        FileByNameMap::iterator i = m_pNewFiles->find(f->name);

        if (i != m_pNewFiles->end())
            m_pNewFiles->erase(i);
        else
            fireFileAdded(f);
    }
    else
    {
        cout << "File with attributtes file: " << f->name << endl;
        m_pFilesTemp->insert( make_pair(f->key, f) );

        FileByIdMap::iterator i = m_pFiles->find(f->key);

        if ( i != m_pFiles->end() )
        {
            cout << "File was already registered" << endl;
            File *pOldFile = (*i).second;

            cout << "Comparing " << pOldFile->name << " vs. " << f->name << endl;
            if ( pOldFile->name.compare(f->name) != 0 )
                fireFileChanged(f);

            m_pFiles->erase(i);
        }
    }
}

void Directory::addSubDirectory(const string& name)
{
    Directory* d = new Directory(m_controller, name, this);
    m_directories.push_back(d);
    m_controller.addDirectory(d);
}

void Directory::scan(void)
{
    struct dirent *entry;

    if (m_dir)
    {
        cout << "scanning " << getPath() << endl;
        rewinddir(m_dir);
        while ( (entry = readdir(m_dir)) )
        {
            if ( !(m_parent == NULL && strcmp(entry->d_name, "cameras") == 0)
                && !(m_parent == NULL && strcmp(entry->d_name, "temp_pvr") == 0)
                && entry->d_name && entry->d_name[0] != '.' )
            {
                if (entry->d_type == DT_REG)
                    checkFile(entry->d_name);
                else if (entry->d_type == DT_DIR)
                    addSubDirectory(entry->d_name);
            }
        }
        fixPairStorages();
    }
}

void Directory::fixPairStorages()
{
    // all the files that are still in the list of are files that have
    // been removed before they have been added to the database
    FileByIdMap::iterator itAttr = m_pFiles->begin();
    while ( itAttr != m_pFiles->end() )
        fireFileRemoved((*itAttr++).second);
    m_pFiles->clear();

    FileByIdMap *pTempData = m_pFiles;
    m_pFiles = m_pFilesTemp;
    m_pFilesTemp = pTempData;

    FileByNameMap::iterator itNew = m_pNewFiles->begin();
    while ( itNew != m_pNewFiles->end() )
        fireFileRemoved((*itNew++).second);
    m_pNewFiles->clear();

    FileByNameMap *pTempDataNew = m_pNewFiles;
    m_pNewFiles = m_pNewFilesTemp;
    m_pNewFilesTemp = pTempDataNew;
}

void Directory::removeChildren(void)
{
/*
    for (FileByIdMap::iterator f = m_files.begin(); f != m_files.end(); ++f)
    {
        m_controller.removeFile((*f).second);
        delete *f;
    }
    m_files.clear();
    for (DirectoryCollection::iterator d = m_directories.begin(); d != m_directories.end(); ++d)
    {
        m_controller.removeDirectory(*d);
        delete *d;
    }
    m_directories.clear();
*/
}

void Directory::rescan(void)
{
//     removeChildrenDirectories();
    scan();
}

void Directory::fireFileRemoved(File *pFile)
{
    m_controller.removeFile(pFile);
}

void Directory::fireFileAdded(File *pFile)
{
    m_controller.addFile(pFile);
}

void Directory::fireFileChanged(File *pFile)
{
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "File changed: %s", (pFile->parent->getPath() + "/" + pFile->name).c_str());
    m_controller.addFile(pFile);
}
