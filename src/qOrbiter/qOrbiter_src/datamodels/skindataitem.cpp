#include "skindataitem.h"


SkinDataItem:: SkinDataItem( const QUrl &url, const QString &name,  const QString &creator, const  QString &description, const QString &version, const  QString &variant , const QImage img, const QString &path, const QString &mainc, const QString &accentc, QObject *parent):
    m_url(url), m_name(name), m_creator(creator), m_description(description), m_version(version), m_variant(variant), m_image(img), m_path(path), m_maincolor(mainc), m_accentcolor(accentc),
    m_styleView(parent)
{

}

QHash<int, QByteArray> SkinDataItem::roleNames() const
{
  QHash<int, QByteArray> names;
  names[NameRole] = "name";
  names[CreatorRole] = "creator";
  names[DescriptionRole] = "description";
  names[VersionRole] = "version";
  names[VariantRole] = "variation";
 names[MainColorRole] = "maincolor";
 names[AccentColorRole] = "accentcolor";
 names[PathRole] = "path";
  names[VariantRole] = "variation";
  names[ImageRole] = "image";

  return names;
}


QVariant SkinDataItem::data(int role) const
{
  switch(role) {
  case NameRole:
    return id();
  case CreatorRole:
    return creator();
  case DescriptionRole:
    return description();
  case VersionRole:
    return version();
  case VariantRole:
    return variant();
  case ImageRole:
    return image();
    case MainColorRole:
      return maincolor();
  case AccentColorRole:
      return accentcolor();
  case PathRole:
      return path();
  default:
    return QVariant();
  }
}
