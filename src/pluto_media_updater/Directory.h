#ifndef _DIRECTORY_H
#define _DIRECTORY_H


#include <string>
#include <vector>

#include <sys/types.h>
#include <dirent.h>
#include <stdint.h>

#include "Controller.h"

using namespace std;

class Controller;
class Directory;

class File
{
    public:
        File(uint64_t key, string name, Directory* parent)
            : key(key), name(name), parent(parent)
            {}

        uint64_t key;
        string name;
        Directory* parent;
};

class Directory
{
    public:
        Directory(Controller& controller, const string& name, Directory* parent);
        ~Directory();
        int getDescriptor() const {return dirfd(m_dir);}
        string getName() const {return m_name;}
        Directory* getParent() const {return m_parent;}
        string getPath() const;

        void checkFile(const string& name);
        void addSubDirectory(const string& name);
        void rescan(void);

    private:
        typedef map<string, File*> FileByNameMap;
        typedef map<uint64_t, File*> FileByIdMap;
        typedef vector<Directory*> DirectoryCollection;

        Controller& m_controller;

        FileByIdMap     *m_pFiles;
        FileByIdMap     *m_pFilesTemp;

        FileByNameMap   *m_pNewFiles;
        FileByNameMap   *m_pNewFilesTemp;

        DirectoryCollection m_directories;
        DIR *m_dir;
        string m_basePath;
        string m_name;
        Directory* m_parent;

        void scan(void);
        void removeChildren(void);
        void fixPairStorages();

    protected:
        void fireFileRemoved(File *pFile);
        void fireFileAdded(File *pFile);
        void fireFileChanged(File *pFile);
};

#endif /* _DIRECTORY_H */
