#ifndef _CONTROLLER_H
#define _CONTROLLER_H


#include <map>
#include <string>

#include <stdint.h>
#include <pthread.h>

#include "Directory.h"

#include "MediaAttributes.h"

using namespace std;

class File;
class Directory;

class Controller
{
    public:
        Controller();
        ~Controller();

        void addDirectory(const string& name);
        void addDirectory(Directory* directory);
        void removeDirectory(Directory* directory);
        void addFile(File* file);
        void removeFile(File* file);
        void rescanDirectory(int fd);
        string getFile(uint64_t key);
        void updateFileKey(string& filename, uint64_t key);

        void setMediaAttributeObject(MediaAttributes *pMediaAttributes);
    private:
        typedef map<int64_t, File*> FileMap;
        typedef map<int, Directory*> DirectoryMap;

        FileMap m_files;
        DirectoryMap m_directories;
        pthread_mutex_t m_files_lock;
        pthread_mutex_t m_directories_lock;
        pthread_mutex_t m_rescan_lock;

        int lastId;

        MediaAttributes *m_pMediaAttr;
};

#endif /* _CONTROLLER_H */
