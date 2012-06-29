#ifndef SKINDATAITEM_H
#define SKINDATAITEM_H

#include <QAbstractListModel>
#include <QVariant>
#include <QImage>
#include <QUrl>
#ifdef debug
#include <QDebug>
#endif


class SkinDataItem: public QObject
{
    Q_OBJECT
    // Q_PROPERTY(QImage buttonImage READ buttonImage NOTIFY imageChanged)

public:
    enum Roles {
        NameRole = Qt::UserRole+1,
        CreatorRole,
        DescriptionRole,
        VersionRole,
        VariantRole,
        MainColorRole,
        AccentColorRole,
        PathRole,
        ImageRole
    };

public:

    SkinDataItem() {}
    explicit SkinDataItem( const QUrl &url, const QString &name,  const QString &creator, const  QString &description, const QString &version, const  QString &variant , const QImage img, const QString &path, const QString &mainc, const QString &accentc, QObject *parent=0);

    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;


    inline QUrl baseUrl() const { return m_url; }
    inline QUrl entryUrl() {
        return QUrl(m_url.toString() + "/main.qml");
    }
    inline QUrl styleUrl() const {
        QUrl style = QUrl(m_url.toString());
        style.setPath(style.toString() + "/Style.qml");

        return style;
    }
    inline QString id() const { return m_path; }
    inline QString creator() const { return m_creator; }
    inline QString description() const { return m_description; }
    inline QString version() const { return m_version; }
    inline QString variant() const { return m_variant; }
    inline QImage image() const {return m_image; }
    inline QString path() const {return m_path;}
    inline QString maincolor() const {return m_maincolor;}
    inline QString accentcolor() const {return m_accentcolor;}
    inline QObject* styleView() const {return m_styleView;}

private:
    QUrl m_url;
    QString m_name;
    QString m_creator;
    QString m_description;
    QString m_version;
    QString m_variant;
    QString m_path;
    QString m_maincolor;
    QString m_accentcolor;
    QImage m_image;
    QObject *m_styleView;

signals:
    void dataChanged();
    void imageChanged();

};

#endif // SKINDATAITEM_H
