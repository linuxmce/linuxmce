#ifndef MEDIAPARSER_H
#define MEDIAPARSER_H

#include <QObject>
#include "items/mediaitem.h"

class MediaParser : public QObject
{
    Q_OBJECT
public:
    explicit MediaParser(QObject *parent = 0); 
    static const QString filter_tv_season;
    static const QString filter_tv_episode;
    static const QString filter_tv_numericEpisode;
    static const QString filter_special_networks_filter;
    static const QString filter_movies;

signals:
    void mediaItemFinished(MediaItem *item);

public slots:

    //work relatedSlots
    MediaItem* identifyMedia(MediaItem *item);




private:




};

#endif // MEDIAPARSER_H
