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
class AttributeTypeHelper : public QObject
{
    Q_OBJECT
public:
    explicit AttributeTypeHelper(QObject *parent = 0);
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
class MediaTypesHelper : public QObject
{
    Q_OBJECT
public:
    MediaTypesHelper() {}
    virtual ~MediaTypesHelper() {}
    enum MediaTypes{
        /*<-AGB->*/
LMCE_LiveTV = 1,
LMCE_CD = 2,
LMCE_DVD = 3,
LMCE_StoredAudio = 4,
LMCE_StoredVideo = 5,
LMCE_LiveRadio = 6,
LMCE_Pictures = 7,
NP_LiveTV = 11,
NP_DVD = 12,
NP_OTARadio = 13,
NP_SatelliteRadio = 14,
NP_CableRadio = 15,
NP_VideoTape = 16,
NP_LaserDisc = 17,
NP_Game = 18,
NP_CD = 19,
MISC_Burning = 20,
MISC_Playlist = 21,
MISC_EditPlaylist = 22,
MISC_BlankMedia = 23,
MISC_DocViewer = 24,
NP_Cassette = 25,
NP_Picture = 26,
LMCE_HDDVD = 27,
LMCE_BD = 28,
LMCE_Game = 29,
LMCE_Game_a2600 = 30,
NP_BluRay = 31,
LMCE_Game_a5200 = 32,
LMCE_Game_a7800 = 33,
LMCE_Game_coleco = 34,
LMCE_Game_intv = 35,
LMCE_Game_famicom = 36,
LMCE_Game_nes = 37,
LMCE_Game_sms = 38,
LMCE_Game_sg1000 = 39,
NP_Tivo3 = 40,
NP_Tivo2 = 41,
LMCE_Hulu = 42,
LMCE_StreamedAudio = 43,
LMCE_StreamedVideo = 44,
LMCE_Game_vectrex = 45,
LMCE_Game_tg16 = 46,
LMCE_Game_genesis = 47,
LMCE_Game_snes = 48,
LMCE_Game_megadriv = 49,
LMCE_Game_snespal = 50,
LMCE_Game_sgx = 51,
LMCE_Game_pce = 52,
LMCE_Game_apple2 = 53,
LMCE_Game_ps1 = 54,
LMCE_Game_ps2 = 55,
LMCE_Game_jaguar = 56,
LMCE_Game_vic20 = 57,
LMCE_Game_c64 = 58,
LMCE_Game_Atari800 = 59,
LMCE_Airplay_audio = 60,
LMCE_AirPlay_video = 61,
LMCE_AirPlay_photos = 62,
LMCE_YouTube = 63,
NP_InternetMedia = 64,
LMCE_Game_amiga = 65,
NP_NetworkMedia = 66,
NP_XBOX360 = 67,
NP_PVR = 68,
NP_Playstation = 69,
LMCE_Game_TI99 = 70
        /*<-AGE->*/
    };
    Q_ENUMS(MediaTypes)
};
#endif // MEDIATYPEHELPER_H
