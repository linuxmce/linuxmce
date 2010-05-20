#ifndef PARSE_H
#define PARSE_H

#include <vector>
using namespace std;

#include <QString>
#include <QList>
#include <QObject>
#include <QProcess>
#include <QDomDocument>
#include <QDateTime>
#include <QPair>
#include <QMap>
#include <QVariant>

class QDomDocument;

   /** Describes an enclosure associated with an item.
    */
   struct Enclosure
   {
       QString URL;
       QString Type;
       qint64 Length;
       QString Lang;
   };

   struct MRSSThumbnail
   {
       QString URL;
       int Width;
       int Height;
       QString Time;
   };

   struct MRSSCredit
   {
       QString Role;
       QString Who;
   };

   struct MRSSComment
   {
       QString Type;
       QString Comment;
   };

   struct MRSSPeerLink
   {
       QString Type;
       QString Link;
   };

   struct MRSSScene
   {
       QString Title;
       QString Description;
       QString StartTime;
       QString EndTime;
   };

   struct MRSSEntry
   {
       QString URL;
       qint64 Size;
       QString Type;
       QString Medium;
       bool IsDefault;
       QString Expression;
       int Bitrate;
       double Framerate;
       double SamplingRate;
       int Channels;
       int Duration;
       int Width;
       int Height;
       QString Lang;
       int Group;
       QString Rating;
       QString RatingScheme;
       QString Title;
       QString Description;
       QString Keywords;
       QString CopyrightURL;
       QString CopyrightText;
       int RatingAverage;
       int RatingCount;
       int RatingMin;
       int RatingMax;
       int Views;
       int Favs;
       QString Tags;
       QList<MRSSThumbnail> Thumbnails;
       QList<MRSSCredit> Credits;
       QList<MRSSComment> Comments;
       QList<MRSSPeerLink> PeerLinks;
       QList<MRSSScene> Scenes;
   };

class ResultVideo
{

  public:

    typedef QList<ResultVideo *> resultList;
    typedef vector<ResultVideo> List;

    ResultVideo(const QString& title, const QString& desc,
              const QString& URL, const QString& thumbnail,
              const QString& mediaURL, const QString& author,
              const QDateTime& date, const QString& time,
              const QString& rating, const off_t& filesize,
              const QString& player, const QStringList& playerargs,
              const QString& download, const QStringList& downloadargs,
              const uint& width, const uint& height, const QString& language,
              const bool& downloadable);
    ResultVideo();
    ~ResultVideo();

    const QString& GetTitle() const { return m_title; }
    const QString& GetDescription() const { return m_desc; }
    const QString& GetURL() const { return m_URL; }
    const QString& GetThumbnail() const { return m_thumbnail; }
    const QString& GetMediaURL() const { return m_mediaURL; }
    const QString& GetAuthor() const { return m_enclosure; }
    const QDateTime& GetDate() const { return m_date; }
    const QString& GetTime() const { return m_time; }
    const QString& GetRating() const { return m_rating; }
    const off_t& GetFilesize() const { return m_filesize; }
    const QString& GetPlayer() const { return m_player; }
    const QStringList& GetPlayerArguments() const { return m_playerargs; }
    const QString& GetDownloader() const { return m_download; }
    const QStringList& GetDownloaderArguments() const { return m_downloadargs; }
    const uint& GetWidth() const { return m_width; }
    const uint& GetHeight() const { return m_height; }
    const QString& GetLanguage() const { return m_language; }
    const bool& GetDownloadable() const { return m_downloadable; }

  private:
    QString   m_title;
    QString   m_desc;
    QString   m_URL;
    QString   m_thumbnail;
    QString   m_mediaURL;
    QString   m_enclosure;
    QDateTime m_date;
    QString   m_time;
    QString   m_rating;
    off_t     m_filesize;
    QString   m_player;
    QStringList  m_playerargs;
    QString   m_download;
    QStringList  m_downloadargs;
    uint      m_width;
    uint      m_height;
    QString   m_language;
    bool      m_downloadable;
};
Q_DECLARE_METATYPE(ResultVideo*)

class Parse : public QObject
{
    Q_OBJECT
    friend class MRSSParser;

  public:
    Parse();
    virtual ~Parse();

    ResultVideo::resultList parseRSS(QDomDocument domDoc);
    ResultVideo* ParseItem(const QDomElement& item) const;

    QString GetLink(const QDomElement&) const;
    QString GetAuthor(const QDomElement&) const;
    QString GetCommentsRSS(const QDomElement&) const;
    QString GetCommentsLink(const QDomElement&) const;
    QDateTime GetDCDateTime(const QDomElement&) const;
    QDateTime FromRFC3339(const QString&) const;
    QDateTime RFC822TimeToQDateTime (const QString&) const;
    int GetNumComments (const QDomElement&) const;
    QStringList GetAllCategories (const QDomElement&) const;
    QPair<double, double> GetGeoPoint (const QDomElement&) const;
    QList<MRSSEntry> GetMediaRSS (const QDomElement&) const;
    QList<Enclosure> GetEnclosures(const QDomElement& entry) const;
    static QString UnescapeHTML (const QString&);

  private:
    QMap<QString, int> TimezoneOffsets;

  protected:
    static const QString DC;
    static const QString WFW;
    static const QString Atom;
    static const QString RDF;
    static const QString Slash;
    static const QString Enc;
    static const QString ITunes;
    static const QString GeoRSSSimple;
    static const QString GeoRSSW3;
    static const QString MediaRSS;

};

#endif
