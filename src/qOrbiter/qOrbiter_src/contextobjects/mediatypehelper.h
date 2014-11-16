#ifndef MEDIATYPEHELPER_H
#define MEDIATYPEHELPER_H

#include <QObject>

/*!
 * \brief The MediaTypeHelper class. this class provides a simple enum
 * for use whe navigating media types.
 * Import into qml with the statement: import org.linuxmce.enums
 * use: Mediatypes.NP_LIVETV
 *
 */
class MediaTypeHelper : public QObject
{
    Q_OBJECT
public:
    explicit MediaTypeHelper(QObject *parent = 0);
    enum MediaTypes{
        LINUXMCE_LIVE_TV=1,
        LINUXMCE_CD=2,
        LINUXMCE_DVD =3,
        STORED_AUDIO=4,
        STORED_VIDEO=5,
        LINUXMCE_LIVE_RADIO=6,
        LINUXMCE_PICTURES=7,
        NP_LIVETV=11,
        NP_DVD=12,
        NP_OTARADIO=13,
        NP_SAT_RADIO=14,
        NP_CABLE_RADIO=15,
        NP_PICTURE =16,
        NP_BluRay_CONST =31,
        LINUXMCE_STREAMED_AUDIO=43,
        LINUXMCE_STREAMED_VIDEO=44,
        LMCE_AIRPLAY_AUDIO=60,
        LMCE_AIRPLAY_VIDEO=61,
        LMCE_AIRPLAY_PHOTOS=62,
        LINUXMCE_YOUTUBE=63,
        NP_INTERNET_MEDIA=64,
        NP_NETWORK_MEDIA=66,
        NP_XBOX_360 = 67,
        NP_PLAYSTATION=68
    };
    Q_ENUMS(MediaTypes)

};

/*!
 * \brief The AttributeTypeHelper class is designed to assist in providing an easy way to decode the options for navigating the media grid.
 * import into qml with the statement: import org.linuxmce.enums
 * use: Attributes.Director
 */
class AttributeTypeHelper : public QObject
{
    Q_OBJECT
public:
    AttributeTypeHelper() {}
    virtual ~AttributeTypeHelper() {}
    enum Attributes{
        Director= 1,
        Performer= 2,
        Album= 3,
        Track= 5,
        ASIN= 6,
        CDDB= 7,
        Genre= 8,
        Ref_Num= 9,
        Channel =10,
        Episode =11,
        Program =12,
        Title =13,
        Disc_ID= 14,
        Conductor= 15,
        ComposerWriter= 16,
        Studio =17,
        Rating =18,
        Release_Date= 19,
        Website =20,
        Chapter =21,
        Encoding =22,
        Format =23,
        Aspect_Ratio_s= 24,
        Audio_Encoding =25,
        Rated =26,
        DVD_Release_Date =27,
        Run_Time =28,
        Keyword =29,
        Screen_Saver_For_MD= 30,
        Region= 32,
        Language= 33,
        Aspect_Ratio= 34,
        Number_of_Discs =35,
        IMDB= 36,
        Synopsis= 37,
        Producer =38,
        Executive_Producer= 39,
        Purchase_Info= 40,
        TV_Channel_ID =41,
        TV_Program_ID= 42,
        TV_Series_ID= 43,
        Creation_Date =44,
        THX_Disk_ID =45,
        Screen_saver_disabled= 46,
        Manufacturer= 47,
        Year= 48,
        System =49,
        Season_Number= 50,
        Episode_Number= 51,
        TV_Season_ID =52,
        System_Configuration= 56
    };
    Q_ENUMS(Attributes)
};

/*!
 * \brief The SubTypesHelper class is for use in qml with the different media subtypes.
 * import into qml with statement: import org.linuxmce.enums.
 * use: MediaSubtpes.TvShows
 */
class SubTypesHelper : public QObject
{
    Q_OBJECT
public:
    SubTypesHelper() {}
    virtual ~SubTypesHelper() {}
    enum MediaSubTypes{
        TVSHOWS=1,
        MOVIES=2,
        HOMEVIDEOS=3,
        SPORTSEVENTS=4,
        MUSICVIDEOS=5,
        ALTERNATIVE=6,
        POPULARMUSIC=7,
        CLASSICALMUSIC=8,
        LEARNING=9,
        AUDIOBOOKS=10,
        ARCADE=11,
        CONSOLE=12
    };
    Q_ENUMS(MediaSubTypes)
};
#endif // MEDIATYPEHELPER_H
