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
    object_title = t;

}
