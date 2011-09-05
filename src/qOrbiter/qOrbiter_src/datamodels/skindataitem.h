#ifndef SKINDATAITEM_H
#define SKINDATAITEM_H

#include <QAbstractListModel>
#include <QVariant>
#include <QImage>

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

          SkinDataItem(QObject *parent = 0) {}
          explicit SkinDataItem( const QString &name,  const QString &creator, const  QString &description, const QString &version, const  QString &variant , QImage img, QString &path, QString &mainc, QString &accentc, QObject *parent=0);

          QVariant data(int role) const;
          QHash<int, QByteArray> roleNames() const;


          inline QString id() const { return m_name; }
          inline QString creator() const { return m_creator; }
          inline QString description() const { return m_description; }
          inline QString version() const { return m_version; }
          inline QString variant() const { return m_variant; }
          inline QImage image() const {return m_image; }
          inline QString path() const {return m_path;}
          inline QString maincolor() const {return m_maincolor;}
          inline QString accentcolor() const {return m_accentcolor;}


        private:
          QString m_name;
          QString m_creator;
          QString m_description;
          QString m_version;
          QString m_variant;
          QString m_path;
          QString m_maincolor;
          QString m_accentcolor;
          QImage m_image;

        signals:
          void dataChanged();
          void imageChanged();

};

#endif // SKINDATAITEM_H
