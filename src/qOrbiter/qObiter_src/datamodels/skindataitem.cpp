#include "skindataitem.h"

SkinDataItem:: SkinDataItem(const QString &name,  const QString &creator, const  QString &description, const QString &version, const  QString &variant, QImage img, QObject *parent):
   m_name(name), m_creator(creator), m_description(description), m_version(version), m_variant(variant), m_image(img)
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
  default:
    return QVariant();
  }
}
