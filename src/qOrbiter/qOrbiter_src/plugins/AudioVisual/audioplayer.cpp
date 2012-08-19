#include "audioplayer.h"

AudioPlayer::AudioPlayer(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);

}
