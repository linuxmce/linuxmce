#include <QFile>
#include <QDataStream>
#include <QDomDocument>
#include <QProcess>
#include <QDomImplementation>
#include <QHash>
#include <QLocale>
#include <QUrl>
#include <QFileInfo>
#include <QRegExp>

#include "parse.h"

#include <mythtv/mythcontext.h>
#include <mythtv/mythdirs.h>

using namespace std;

ResultVideo::ResultVideo(const QString& title, const QString& desc,
              const QString& URL, const QString& thumbnail,
              const QString& mediaURL, const QString& enclosure,
              const QDateTime& date, const QString& time,
              const QString& rating, const off_t& filesize,
              const QString& player, const QStringList& playerargs,
              const QString& download, const QStringList& downloadargs,
              const uint& width, const uint& height,
              const QString& language, const bool& downloadable)
{
    m_title = title;
    m_desc = desc;
    m_URL = URL;
    m_thumbnail = thumbnail;
    m_mediaURL = mediaURL;
    m_enclosure = enclosure;
    m_date = date;
    m_time = time;
    m_rating = rating;
    m_filesize = filesize;
    m_player = player;
    m_playerargs = playerargs;
    m_download = download;
    m_downloadargs = downloadargs;
    m_width = width;
    m_height = height;
    m_language = language;
    m_downloadable = downloadable;
}

ResultVideo::ResultVideo()
{
}

ResultVideo::~ResultVideo()
{
}

namespace
{
        QList<QDomNode> GetDirectChildrenNS(const QDomElement& elem,
                        const QString& ns, const QString& name)
        {
                QList<QDomNode> result;
                QDomNodeList unf = elem.elementsByTagNameNS(ns, name);
                for (int i = 0, size = unf.size(); i < size; ++i)
                        if (unf.at(i).parentNode() == elem)
                                result << unf.at(i);
                return result;
        }
}

class MRSSParser
{
    struct ArbitraryLocatedData
    {
        QString URL;
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

        /**  Updates *this's fields according to the
         * child. Some kind of merge.
         */
        ArbitraryLocatedData& operator+= (const ArbitraryLocatedData& child)
        {
            if (!child.URL.isEmpty())
                URL = child.URL;
            if (!child.Rating.isEmpty())
                Rating = child.Rating;
            if (!child.RatingScheme.isEmpty())
                RatingScheme = child.RatingScheme;
            if (!child.Title.isEmpty())
                Title = child.Title;
            if (!child.Description.isEmpty())
                Description = child.Description;
            if (!child.Keywords.isEmpty())
                Keywords = child.Keywords;
            if (!child.CopyrightURL.isEmpty())
                CopyrightURL = child.CopyrightURL;
            if (!child.CopyrightText.isEmpty())
                CopyrightText = child.CopyrightText;
            if (child.RatingAverage != 0)
                RatingAverage = child.RatingAverage;
            if (child.RatingCount != 0)
                RatingCount = child.RatingCount;
            if (child.RatingMin != 0)
                RatingMin = child.RatingMin;
            if (child.RatingMax != 0)
                RatingMax = child.RatingMax;
            if (child.Views != 0)
                Views = child.Views;
            if (child.Favs != 0)
                Favs = child.Favs;
            if (!child.Tags.isEmpty())
                Tags = child.Tags;

            Thumbnails += child.Thumbnails;
            Credits += child.Credits;
            Comments += child.Comments;
            PeerLinks += child.PeerLinks;
            Scenes += child.Scenes;
            return *this;
        }
    };


public:
    MRSSParser() {}

    QList<MRSSEntry> operator() (const QDomElement& item)
    {
        QList<MRSSEntry> result;

        QDomNodeList groups = item.elementsByTagNameNS(QString("http://search.yahoo.com/mrss/"),
            "group");

        for (int i = 0; i < groups.size(); ++i)
            result += CollectChildren(groups.at(i).toElement());

        result += CollectChildren(item);

        return result;
    }

private:

    QList<MRSSEntry> CollectChildren(const QDomElement& holder)
    {
         QList<MRSSEntry> result;
         QDomNodeList entries = holder.elementsByTagNameNS(Parse::MediaRSS,
             "content");

         for (int i = 0; i < entries.size(); ++i)
         {
             MRSSEntry entry;

             QDomElement en = entries.at(i).toElement();
             ArbitraryLocatedData d = GetArbitraryLocatedDataFor(en);

             if (en.hasAttribute("url"))
                 entry.URL = en.attribute("url");
             else
                 entry.URL = d.URL;

             entry.Size = en.attribute("fileSize").toInt();
             entry.Type = en.attribute("type");
             entry.Medium = en.attribute("medium");
             entry.IsDefault = (en.attribute("isDefault") == "true");
             entry.Expression = en.attribute("expression");
             if (entry.Expression.isEmpty())
                 entry.Expression = "full";
             entry.Bitrate = en.attribute("bitrate").toInt();
             entry.Framerate = en.attribute("framerate").toDouble();
             entry.SamplingRate = en.attribute("samplingrate").toDouble();
             entry.Channels = en.attribute("channels").toInt();
             entry.Duration = en.attribute("duration").toInt();
             if (!en.attribute("width").isNull())
                 entry.Width = en.attribute("width").toInt();
             else
                 entry.Width = 0;
             if (!en.attribute("height").isNull())
                 entry.Height = en.attribute("height").toInt();
             else
                 entry.Height = 0;
             if (!en.attribute("lang").isNull())
                 entry.Lang = en.attribute("lang");
             else
                 entry.Lang = QString();

             entry.Rating = d.Rating;
             entry.RatingScheme = d.RatingScheme;
             entry.Title = d.Title;
             entry.Description = d.Description;
             entry.Keywords = d.Keywords;
             entry.CopyrightURL = d.CopyrightURL;
             entry.CopyrightText = d.CopyrightText;
             if (d.RatingAverage != 0)
                 entry.RatingAverage = d.RatingAverage;
             else
                 entry.RatingAverage = 0;
             entry.RatingCount = d.RatingCount;
             entry.RatingMin = d.RatingMin;
             entry.RatingMax = d.RatingMax;
             entry.Views = d.Views;
             entry.Favs = d.Favs;
             entry.Tags = d.Tags;
             entry.Thumbnails = d.Thumbnails;
             entry.Credits = d.Credits;
             entry.Comments = d.Comments;
             entry.PeerLinks = d.PeerLinks;
             entry.Scenes = d.Scenes;

             result << entry;
        }
        return result;
    }

    ArbitraryLocatedData GetArbitraryLocatedDataFor(const QDomElement& holder)
    {
        ArbitraryLocatedData result;

        QList<QDomElement> parents;
        QDomElement parent = holder;
        while (!parent.isNull())
        {
            parents.prepend(parent);
            parent = parent.parentNode().toElement();
        }

        Q_FOREACH(QDomElement p, parents)
            result += CollectArbitraryLocatedData(p);

        return result;
    }

    QString GetURL(const QDomElement& element)
    {
        QList<QDomNode> elems = GetDirectChildrenNS(element, Parse::MediaRSS,
            "player");
        if (!elems.size())
            return QString();

        return QString(elems.at(0).toElement().attribute("url"));
    }

    QString GetTitle(const QDomElement& element)
    {
        QList<QDomNode> elems = GetDirectChildrenNS(element, Parse::MediaRSS,
            "title");

        if (!elems.size())
            return QString();

        QDomElement telem = elems.at(0).toElement();
        return QString(Parse::UnescapeHTML(telem.text()));
    }

    QString GetDescription(const QDomElement& element)
    {
        QList<QDomNode> elems = GetDirectChildrenNS(element, Parse::MediaRSS,
            "description");

        if (!elems.size())
            return QString();

        QDomElement telem = elems.at(0).toElement();
        return QString(Parse::UnescapeHTML(telem.text()));
    }

    QString GetKeywords(const QDomElement& element)
    {
        QList<QDomNode> elems = GetDirectChildrenNS(element, Parse::MediaRSS,
            "keywords");

        if (!elems.size())
            return QString();

        QDomElement telem = elems.at(0).toElement();
        return QString(telem.text());
    }

    int GetInt(const QDomElement& elem, const QString& attrname)
    {
        if (elem.hasAttribute(attrname))
        {
            bool ok = false;
            int result = elem.attribute(attrname).toInt(&ok);
            if (ok)
                return int(result);
        }
        return int();
    }

    QList<MRSSThumbnail> GetThumbnails(const QDomElement& element)
    {
        QList<MRSSThumbnail> result;
        QList<QDomNode> thumbs = GetDirectChildrenNS(element, Parse::MediaRSS,
            "thumbnail");
        for (int i = 0; i < thumbs.size(); ++i)
        {
            QDomElement thumbNode = thumbs.at(i).toElement();
            int widthOpt = GetInt(thumbNode, "width");
            int width = widthOpt ? widthOpt : 0;
            int heightOpt = GetInt(thumbNode, "height");
            int height = heightOpt ? heightOpt : 0;
            MRSSThumbnail thumb =
            {
                thumbNode.attribute("url"),
                width,
                height,
                thumbNode.attribute("time")
             };
             result << thumb;
        }
        return result;
    }

    QList<MRSSCredit> GetCredits(const QDomElement& element)
    {
        QList<MRSSCredit> result;
        QList<QDomNode> credits = GetDirectChildrenNS(element, Parse::MediaRSS,
           "credit");

        for (int i = 0; i < credits.size(); ++i)
        {
            QDomElement creditNode = credits.at(i).toElement();
            if (!creditNode.hasAttribute("role"))
                 continue;
            MRSSCredit credit =
            {
                creditNode.attribute("role"),
                creditNode.text()
            };
            result << credit;
        }
        return result;
    }

    QList<MRSSComment> GetComments(const QDomElement& element)
    {
        QList<MRSSComment> result;
        QList<QDomNode> commParents = GetDirectChildrenNS(element, Parse::MediaRSS,
            "comments");

        if (commParents.size())
        {
            QDomNodeList comments = commParents.at(0).toElement()
                .elementsByTagNameNS(Parse::MediaRSS,
                "comment");
            for (int i = 0; i < comments.size(); ++i)
            {
                MRSSComment comment =
                {
                    QObject::tr("Comments"),
                    comments.at(i).toElement().text()
                };
                result << comment;
            }
        }

        QList<QDomNode> respParents = GetDirectChildrenNS(element, Parse::MediaRSS,
            "responses");

        if (respParents.size())
        {
            QDomNodeList responses = respParents.at(0).toElement()
                .elementsByTagNameNS(Parse::MediaRSS,
                "response");
            for (int i = 0; i < responses.size(); ++i)
            {
                MRSSComment comment =
                {
                    QObject::tr("Responses"),
                    responses.at(i).toElement().text()
                };
                result << comment;
            }
        }

        QList<QDomNode> backParents = GetDirectChildrenNS(element, Parse::MediaRSS,
            "backLinks");

        if (backParents.size())
        {
            QDomNodeList backlinks = backParents.at(0).toElement()
                .elementsByTagNameNS(Parse::MediaRSS,
                "backLink");
            for (int i = 0; i < backlinks.size(); ++i)
            {
                MRSSComment comment =
                {
                    QObject::tr("Backlinks"),
                    backlinks.at(i).toElement().text()
                };
                result << comment;
            }
        }
        return result;
    }

    QList<MRSSPeerLink> GetPeerLinks(const QDomElement& element)
    {
        QList<MRSSPeerLink> result;
        QList<QDomNode> links = GetDirectChildrenNS(element, Parse::MediaRSS,
            "peerLink");

        for (int i = 0; i < links.size(); ++i)
        {
            QDomElement linkNode = links.at(i).toElement();
            MRSSPeerLink pl =
            {
                linkNode.attribute("type"),
                linkNode.attribute("href")
            };
            result << pl;
        }
        return result;
    }

    QList<MRSSScene> GetScenes(const QDomElement& element)
    {
        QList<MRSSScene> result;
        QList<QDomNode> scenesNode = GetDirectChildrenNS(element, Parse::MediaRSS,
            "scenes");

        if (scenesNode.size())
        {
            QDomNodeList scenesNodes = scenesNode.at(0).toElement()
                .elementsByTagNameNS(Parse::MediaRSS, "scene");

            for (int i = 0; i < scenesNodes.size(); ++i)
            {
                QDomElement sceneNode = scenesNodes.at(i).toElement();
                MRSSScene scene =
                {
                    sceneNode.firstChildElement("sceneTitle").text(),
                    sceneNode.firstChildElement("sceneDescription").text(),
                    sceneNode.firstChildElement("sceneStartTime").text(),
                    sceneNode.firstChildElement("sceneEndTime").text()
                };
                result << scene;
            }
        }
        return result;
    }

    ArbitraryLocatedData CollectArbitraryLocatedData(const QDomElement& element)
    {

        QString rating;
        QString rscheme;
        {
            QList<QDomNode> elems = GetDirectChildrenNS(element, Parse::MediaRSS,
                "rating");

            if (elems.size())
            {
                QDomElement relem = elems.at(0).toElement();
                rating = relem.text();
                if (relem.hasAttribute("scheme"))
                    rscheme = relem.attribute("scheme");
                else
                    rscheme = "urn:simple";
            }
        }

        QString curl;
        QString ctext;
        {
            QList<QDomNode> elems = GetDirectChildrenNS(element, Parse::MediaRSS,
                "copyright");

            if (elems.size())
            {
                QDomElement celem = elems.at(0).toElement();
                ctext = celem.text();
                if (celem.hasAttribute("url"))
                    curl = celem.attribute("url");
            }
        }

        int raverage = 0;
        int rcount = 0;
        int rmin = 0;
        int rmax = 0;
        int views = 0;
        int favs = 0;
        QString tags;
        {
            QList<QDomNode> comms = GetDirectChildrenNS(element, Parse::MediaRSS,
                "community");
            if (comms.size())
            {
                QDomElement comm = comms.at(0).toElement();
                QDomNodeList stars = comm.elementsByTagNameNS(Parse::MediaRSS,
                    "starRating");
                if (stars.size())
                {
                    QDomElement rating = stars.at(0).toElement();
                    raverage = GetInt(rating, "average");
                    rcount = GetInt(rating, "count");
                    rmin = GetInt(rating, "min");
                    rmax = GetInt(rating, "max");
                }

                QDomNodeList stats = comm.elementsByTagNameNS(Parse::MediaRSS,
                    "statistics");
                if (stats.size())
                {
                    QDomElement stat = stats.at(0).toElement();
                    views = GetInt(stat, "views");
                    favs = GetInt(stat, "favorites");
                 }

                QDomNodeList tagsNode = comm.elementsByTagNameNS(Parse::MediaRSS,
                    "tags");
                if (tagsNode.size())
                {
                    QDomElement tag = tagsNode.at(0).toElement();
                    tags = tag.text();
                }
            }
        }

        ArbitraryLocatedData result =
        {
            GetURL(element),
            rating,
            rscheme,
            GetTitle(element),
            GetDescription(element),
            GetKeywords(element),
            curl,
            ctext,
            raverage,
            rcount,
            rmin,
            rmax,
            views,
            favs,
            tags,
            GetThumbnails(element),
            GetCredits(element),
            GetComments(element),
            GetPeerLinks(element),
            GetScenes(element)
        };

        return result;
    }
};


//========================================================================================
//          Search Construction, Destruction
//========================================================================================

const QString Parse::DC = "http://purl.org/dc/elements/1.1/";
const QString Parse::WFW = "http://wellformedweb.org/CommentAPI/";
const QString Parse::Atom = "http://www.w3.org/2005/Atom";
const QString Parse::RDF = "http://www.w3.org/1999/02/22-rdf-syntax-ns#";
const QString Parse::Slash = "http://purl.org/rss/1.0/modules/slash/";
const QString Parse::Enc = "http://purl.oclc.org/net/rss_2.0/enc#";
const QString Parse::ITunes = "http://www.itunes.com/dtds/podcast-1.0.dtd";
const QString Parse::GeoRSSSimple = "http://www.georss.org/georss";
const QString Parse::GeoRSSW3 = "http://www.w3.org/2003/01/geo/wgs84_pos#";
const QString Parse::MediaRSS = "http://search.yahoo.com/mrss/";

Parse::Parse()
{
}

Parse::~Parse()
{
}

ResultVideo::resultList Parse::parseRSS(QDomDocument domDoc)
{
    ResultVideo::resultList vList;

    QString document = domDoc.toString();
    VERBOSE(VB_GENERAL|VB_EXTRA, QString("Will Be Parsing: %1").arg(document));

    QDomElement root = domDoc.documentElement();
    QDomElement channel = root.firstChildElement("channel");
    while (!channel.isNull())
    {
        QDomElement item = channel.firstChildElement("item");
        while (!item.isNull())
        {
            vList.append(ParseItem(item));
            item = item.nextSiblingElement("item");
        }
        channel = channel.nextSiblingElement("channel");
    }

    return vList;
}

ResultVideo* Parse::ParseItem(const QDomElement& item) const
{
    QString title, description, url, author, duration, rating,
            thumbnail, mediaURL, player, language, download = NULL;
    off_t filesize = 0;
    uint width, height = 0;
    QDateTime date;
    QStringList playerargs, downloadargs;
    bool downloadable = true;

    title = item.firstChildElement("title").text();
    title = UnescapeHTML(title);
    if (title.isEmpty())
        title = "";

    description = item.firstChildElement("description").text();
    if (description.isEmpty())
    {
        QDomNodeList nodes = item.elementsByTagNameNS(ITunes, "summary");
        if (nodes.size())
            description = nodes.at(0).toElement().text();
    }
    if (description.isEmpty())
        description = "";
    else
        description = UnescapeHTML(description);

    url = item.firstChildElement("link").text();

    author = item.firstChildElement("author").text();
    if (author.isEmpty())
        author = GetAuthor(item);

    date = RFC822TimeToQDateTime(item.firstChildElement("pubDate").text());
    if (!date.isValid() || date.isNull())
        date = GetDCDateTime(item);
    if (!date.isValid() || date.isNull())
        date = QDateTime::currentDateTime();

    QDomNodeList dur = item.elementsByTagNameNS(ITunes, "duration");
    if (dur.size())
    {
        duration = dur.at(0).toElement().text();
    }

    rating = item.firstChildElement("rating").text();

    player = item.firstChildElement("player").text();
    playerargs = item.firstChildElement("playerargs").text().split(" ");
    download = item.firstChildElement("download").text();
    downloadargs = item.firstChildElement("downloadargs").text().split(" ");

    QList<MRSSEntry> enclosures = GetMediaRSS(item);

    if (enclosures.size())
    {
        MRSSEntry media = enclosures.takeAt(0);

        QList<MRSSThumbnail> thumbs = media.Thumbnails;
        if (thumbs.size())
        {
            MRSSThumbnail thumb = thumbs.takeAt(0);
            thumbnail = thumb.URL;
        }

        mediaURL = media.URL;

        width = media.Width;
        height = media.Height;
        language = media.Lang;

        if (duration.isEmpty())
            duration = media.Duration;

        if (filesize == 0)
            filesize = media.Size;

        if (rating.isEmpty())
            rating = QString::number(media.RatingAverage);
    }
    if (mediaURL.isEmpty())
    {
        QList<Enclosure> stdEnc = GetEnclosures(item);

        if (stdEnc.size())
        {
            Enclosure e = stdEnc.takeAt(0);

            mediaURL = e.URL;

            if (filesize == 0)
                filesize = e.Length;
        }
    }

    if (mediaURL.isNull() || mediaURL == url)
        downloadable = false;

    return(new ResultVideo(title, description, url, thumbnail,
              mediaURL, author, date, duration,
              rating, filesize, player, playerargs,
              download, downloadargs, width, height,
              language, downloadable));
}

QString Parse::GetLink(const QDomElement& parent) const
{
    QString result;
    QDomElement link = parent.firstChildElement("link");
    while(!link.isNull())
    {
        if (!link.hasAttribute("rel") || link.attribute("rel") == "alternate")
        {
            if (!link.hasAttribute("href"))
                result = link.text();
            else
                result = link.attribute("href");
            break;
        }
        link = link.nextSiblingElement("link");
    }
    return result;
}

QString Parse::GetAuthor(const QDomElement& parent) const
{
    QString result;
    QDomNodeList nodes = parent.elementsByTagNameNS(ITunes,
        "author");
    if (nodes.size())
    {
        result = nodes.at(0).toElement().text();
        return result;
    }

    nodes = parent.elementsByTagNameNS(DC,
       "creator");
    if (nodes.size())
    {
        result = nodes.at(0).toElement().text();
        return result;
    }

    return result;
}

QString Parse::GetCommentsRSS(const QDomElement& parent) const
{
    QString result;
    QDomNodeList nodes = parent.elementsByTagNameNS(WFW,
        "commentRss");
    if (nodes.size())
        result = nodes.at(0).toElement().text();
    return result;
}

QString Parse::GetCommentsLink(const QDomElement& parent) const
{
    QString result;
    QDomNodeList nodes = parent.elementsByTagNameNS("", "comments");
    if (nodes.size())
        result = nodes.at(0).toElement().text();
    return result;
}

QDateTime Parse::GetDCDateTime(const QDomElement& parent) const
{
    QDomNodeList dates = parent.elementsByTagNameNS(DC, "date");
    if (!dates.size())
        return QDateTime();
    return FromRFC3339(dates.at(0).toElement().text());
}

QDateTime Parse::RFC822TimeToQDateTime(const QString& t) const
{
    if (t.size() < 20)
        return QDateTime();

    QString time = t.simplified();
    short int hoursShift = 0, minutesShift = 0;

    QStringList tmp = time.split(' ');
    if (tmp.isEmpty())
        return QDateTime();
    if (tmp. at(0).contains(QRegExp("\\D")))
        tmp.removeFirst();
    if (tmp.size() != 5)
        return QDateTime();
    QString timezone = tmp.takeAt(tmp.size() -1);
    if (timezone.size() == 5)
    {
        bool ok;
        int tz = timezone.toInt(&ok);
        if(ok)
        {
            hoursShift = tz / 100;
            minutesShift = tz % 100;
        }
    }
    else
        hoursShift = TimezoneOffsets.value(timezone, 0);

    if (tmp.at(0).size() == 1)
        tmp[0].prepend("0");
    tmp [1].truncate(3);

    time = tmp.join(" ");

    QDateTime result;
    if (tmp.at(2).size() == 4)
        result = QLocale::c().toDateTime(time, "dd MMM yyyy hh:mm:ss");
    else
        result = QLocale::c().toDateTime(time, "dd MMM yy hh:mm:ss");
    if (result.isNull() || !result.isValid())
        return QDateTime();
    result = result.addSecs(hoursShift * 3600 * (-1) + minutesShift *60 * (-1));
    result.setTimeSpec(Qt::UTC);
    return result.toLocalTime();
}

QDateTime Parse::FromRFC3339(const QString& t) const
{
    int hoursShift = 0, minutesShift = 0;
    if (t.size() < 19)
        return QDateTime();
    QDateTime result = QDateTime::fromString(t.left(19).toUpper(), "yyyy-MM-ddTHH:mm:ss");
    QRegExp fractionalSeconds("(\\.)(\\d+)");
    if (fractionalSeconds.indexIn(t) > -1)
    {
        bool ok;
        int fractional = fractionalSeconds.cap(2).toInt(&ok);
        if (ok)
        {
            if (fractional < 100)
                fractional *= 10;
            if (fractional <10)
                fractional *= 100;
            result.addMSecs(fractional);
        }
    }
    QRegExp timeZone("(\\+|\\-)(\\d\\d)(:)(\\d\\d)$");
    if (timeZone.indexIn(t) > -1)
    {
        short int multiplier = -1;
        if (timeZone.cap(1) == "-")
            multiplier = 1;
        hoursShift = timeZone.cap(2).toInt();
        minutesShift = timeZone.cap(4).toInt();
        result = result.addSecs(hoursShift * 3600 * multiplier + minutesShift * 60 * multiplier);
    }
    result.setTimeSpec(Qt::UTC);
    return result.toLocalTime();
}

QList<Enclosure> Parse::GetEnclosures(const QDomElement& entry) const
{
    QList<Enclosure> result;
    QDomNodeList links = entry.elementsByTagName("enclosure");
    for (int i = 0; i < links.size(); ++i)
    {
        QDomElement link = links.at(i).toElement();

        Enclosure e =
        {
            link.attribute("url"),
            link.attribute("type"),
            link.attribute("length", "-1").toLongLong(),
            link.attribute("hreflang")
        };

        result << e;
    }
    return result;
}

QList<MRSSEntry> Parse::GetMediaRSS(const QDomElement& item) const
{
    return MRSSParser() (item);
}

QString Parse::UnescapeHTML(const QString& escaped)
{
    QString result = escaped;
    result.replace("&amp;", "&");
    result.replace("&lt;", "<");
    result.replace("&gt;", ">");
    result.replace("&apos;", "\'");
    result.replace("&rsquo;", "\'");
    result.replace("&#x2019;", "\'");
    result.replace("&quot;", "\"");
    result.replace("&#8230;",QChar(8230));
    result.replace("&#233;",QChar(233));
    result.replace("&mdash;", QChar(8212));
    result.replace("&nbsp;", " ");
    result.replace("&#160;", QChar(160));
    result.replace("&#225;", QChar(225));
    result.replace("&#8216;", QChar(8216));
    result.replace("&#8217;", QChar(8217));
    result.replace("&#039;", "\'");
    result.replace("&ndash;", QChar(8211));
    result.replace("&auml;", QChar(0x00e4));
    result.replace("&ouml;", QChar(0x00f6));
    result.replace("&uuml;", QChar(0x00fc));
    result.replace("&Auml;", QChar(0x00c4));
    result.replace("&Ouml;", QChar(0x00d6));
    result.replace("&Uuml;", QChar(0x00dc));
    result.replace("&szlig;", QChar(0x00df));
    result.replace("&euro;", "€");
    result.replace("&#8230;", "...");
    result.replace("&#x00AE;", QChar(0x00ae));
    result.replace("&#x201C;", QChar(0x201c));
    result.replace("&#x201D;", QChar(0x201d));
    result.replace("<p>", "\n");

    QRegExp stripHTML(QLatin1String("<.*>"));
    stripHTML.setMinimal(true);
    result.remove(stripHTML);

    return result;
}
