/*
        LinuxMCE autotagger
        Langston Ball golgoj4@gmail.com
        GPLv2 Licensed
        LinuxMCE auto-metadata lookup
        Utilizes www.thetvdb.com and
        www.themoviedb.com to find
        relevant information and populate the
        linuxmce Database
*/

#ifndef DATABASEFUNCTIONS_H
#define DATABASEFUNCTIONS_H
#include <autotagger.h>
#include <mysql/mysql.h>
#include <QMYSQLDriver>
#include <qsql_mysql.h>


class databaseFunctions: public QObject
{
    Q_OBJECT
public:
    databaseFunctions();
    QSqlDatabase lmceDB;
    QString appPath;
    int isDir;
    QStringList directoryFiles;
    QStringList directoryFiles_ID;
    QStringList subDirectoryFiles;
    QStringList subDirectoryFiles_ID;

    QString findFileNo(QString fullFilePath, QString fullFileName);
    QString checkAttribute(QString attribute, QString fileNo, int AttributeType);
    QString searchAttribute(QString file, int attributeType);
    int findMediaType(QString file);

    QString insertAttribute(QString ins_attribute, QString inc_attribType);
    int associateFile(QString fkFile, QString fkAttributeNo );
    int connectDB();
    int savePic(QString picUrl, QString pic_Attribute, QString file_num);
    int saveAttributePic(QString picUrl, QString attribute);
    int connectAttribute(QString inc_attrib_no, QString existing_fileID);
    QString findHome(QString arg);
    QString findHome_int(QString arg);

    QString feedMe(QString directoryPath);
    void closeDB();
    void updateSynopsis(QString synop, QString FileID, int atribEnum);
    void setMediaTypes (QString file_format, QString sub_Type, QString File_Num);
    QFile logFile;
    QTextStream logThis;


private:
    QString databaseName;
    QString dbTable;
    QString pass;
    QString user;
};

#endif // DATABASEFUNCTIONS_H
