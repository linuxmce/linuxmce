#include "mediaparser.h"

const QString MediaParser::filter_movies = ".[1-9]\\d\\d\\d.(?!.S)| \\d\\d\\d\\d(?!.\\d)|[1-9][0-9][0-9][0-9](?!.S)" ;
const QString MediaParser::filter_tv_season = "pt|s[0-9]|s[0-9][0-9]|season\\s\\d";
const QString MediaParser::filter_tv_episode = "e[0-9]|e[0-9][0-9]|ep[0-9]|ep[0-9]|episode_\\D";
const QString MediaParser::filter_tv_numericEpisode = "[0-9]x[0-9]| \\b\\d\\d\\d\\b";
const QString MediaParser::filter_special_networks_filter = "\\dof\\d|BBC|PBS";

MediaParser::MediaParser(QObject *parent) : QObject(parent)
{

}

MediaItem *MediaParser::identifyMedia(MediaItem *item)
{

    return item;
}



