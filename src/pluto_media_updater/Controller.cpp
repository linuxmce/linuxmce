#include <iostream>

#include <glib.h>
#include "Controller.h"

#include "pluto_media/Define_Type.h"

Controller::Controller()
{
    pthread_mutex_init(&m_files_lock, NULL);
    pthread_mutex_init(&m_directories_lock, NULL);
    pthread_mutex_init(&m_rescan_lock, NULL);
}

Controller::~Controller()
{
    pthread_mutex_destroy(&m_files_lock);
    pthread_mutex_destroy(&m_directories_lock);
    pthread_mutex_destroy(&m_rescan_lock);
}


void Controller::addDirectory(const string& name)
{
    Directory* dir = new Directory(*this, name, NULL);
    pthread_mutex_lock(&m_directories_lock);
    m_directories.insert( make_pair(dir->getDescriptor(), dir) );
    pthread_mutex_unlock(&m_directories_lock);
}

void Controller::addDirectory(Directory* directory)
{
    pthread_mutex_lock(&m_directories_lock);
    m_directories.insert( make_pair(directory->getDescriptor(), directory) );
    pthread_mutex_unlock(&m_directories_lock);
}

void Controller::removeDirectory(Directory* directory)
{
    DirectoryMap::iterator i;
    pthread_mutex_lock(&m_directories_lock);
    i = m_directories.find(directory->getDescriptor());
    if (i != m_directories.end())
        m_directories.erase(i);
    pthread_mutex_unlock(&m_directories_lock);
}

void Controller::addFile(File* file)
{
    pthread_mutex_lock(&m_files_lock);
    int m_iFileID;

    if  ( (m_iFileID = m_pMediaAttr->GetFileIDFromFilePath(file->parent->getPath() + "/" + file->name) ) == 0 )
        m_pMediaAttr->CreatedMedia(TYPE_Music_CONST, file->parent->getPath() + "/" + file->name, NULL, NULL);
    else
        m_pMediaAttr->UpdatedMedia(m_iFileID, TYPE_Music_CONST, file->parent->getPath() + "/" + file->name, NULL, NULL);

    pthread_mutex_unlock(&m_files_lock);
}

void Controller::removeFile(File* file)
{
    pthread_mutex_lock(&m_files_lock);
    if ( file->key != 0 )
        m_pMediaAttr->MarkAsMissing(file->key, file->name);

    pthread_mutex_unlock(&m_files_lock);
}

void Controller::rescanDirectory(int fd)
{
    DirectoryMap::iterator i;
    pthread_mutex_lock(&m_directories_lock);
    pthread_mutex_lock(&m_rescan_lock);

    i = m_directories.find(fd);
    if (i != m_directories.end())
    {
        pthread_mutex_unlock(&m_directories_lock);
        cout << i->second->getPath() << " changed\n";
        i->second->rescan();
    }
    else
        pthread_mutex_unlock(&m_directories_lock);
    pthread_mutex_unlock(&m_rescan_lock);
}


string Controller::getFile(uint64_t key)
{
    FileMap::iterator i;
    string result = "";

    pthread_mutex_lock(&m_files_lock);
    i = m_files.find(key);
    if (i != m_files.end())
        result = i->second->parent->getPath() + "/" + i->second->name;
    pthread_mutex_unlock(&m_files_lock);
    return result;
}

void Controller::updateFileKey(string& filename, uint64_t key)
{
    FileMap::iterator i;
    char *dirname = g_path_get_dirname(filename.c_str());

    if (dirname)
    {
        string directory = string(dirname);
        pthread_mutex_lock(&m_files_lock);
        for (i = m_files.begin(); i != m_files.end(); ++i)
        {
            File *file = i->second;
            char *base = basename(filename.c_str());
            if (base && file->name == string(base) &&
                file->parent->getPath() == directory)
            {
                FileMap::iterator j;
                j = m_files.find(file->key);
                if (j != m_files.end())
                    m_files.erase(j);
                file->key = key;
                cout << "updating file " << file->parent->getPath() << "/" << file->name << " with ID " << file->key << endl;
                if (key > 0)
                    m_files.insert( make_pair(file->key, file) );
                break;
            }
        }
        pthread_mutex_unlock(&m_files_lock);
        if (i == m_files.end())
        {
            DirectoryMap::iterator i;
            pthread_mutex_lock(&m_directories_lock);
            for (i = m_directories.begin(); i != m_directories.end(); ++i)
            {
                Directory *dir = i->second;
                if (directory == dir->getPath())
                {
                    pthread_mutex_unlock(&m_directories_lock);
                    dir->rescan();
                    pthread_mutex_lock(&m_directories_lock);
                    break;
                }
            }
            pthread_mutex_unlock(&m_directories_lock);
        }
        free(dirname);
    }
}

void Controller::setMediaAttributeObject(MediaAttributes *pMediaAttributes)
{
    this->m_pMediaAttr = pMediaAttributes;
}
