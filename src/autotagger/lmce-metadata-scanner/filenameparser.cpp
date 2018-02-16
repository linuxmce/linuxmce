#include "filenameparser.h"

FilenameParser::FilenameParser(QObject *parent) : QObject(parent)
{
 setupFilters();
}

void FilenameParser::parseFileName(QString fileName)
{

}

void FilenameParser::parseFileNameWithData(QString name, int mediaType, int mediaSubtype)
{

}

bool FilenameParser::setupFilters()
{
    m_videoFileNames << "mov" << "avi" << "mpeg" << "mpg" << "wmv" << "ts" << "tp" << "dvd" << "vdr" << "ogm" << "evo" << "m2ts" << "iso" << "mp4" << "divx" << "blu";

    return true;
}

