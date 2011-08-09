#include "filedetailsclass.h"

FileDetailsClass::FileDetailsClass(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{
}

void FileDetailsClass::setFileMediaType()
{
}

void FileDetailsClass::setTitle(QString t)
{
    objecttitle = t;
    emit object_changed();

}
