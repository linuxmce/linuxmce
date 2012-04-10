#include "bookmarkitem.h"

BookmarkItem::BookmarkItem(QString title, QImage incMediaImage, QImage incBookmarkImage, QObject *parent) :
    QObject(parent), title(title), mediaImage(incMediaImage), bookmarkImage(incBookmarkImage)
{
}
