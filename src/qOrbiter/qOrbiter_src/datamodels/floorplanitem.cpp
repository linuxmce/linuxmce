#include "floorplanimageitem.h"

FloorPlanItem::FloorPlanItem(QString &installation, QString &description,  int &page,  QString &icon, QImage fpImage,  QImage &icoimg, QObject *parent) :
    m_installation(installation), m_description(description), m_page(page), m_iconPath(icon), m_floorplanImage(fpImage), m_iconImage(icoimg)
{
    m_ident = installation + "-" + QString::number(page) ;

}

QHash<int, QByteArray> FloorPlanItem::roleNames() const
{

  QHash<int, QByteArray> names;
  names[InstallationRole] = "installation";
  names[DescriptionRole] = "description";
  names[PageRole] = "page";
  names[IconRole] = "iconpath";
  names[FloorPlanImageRole] = "floorplanimage";
  names[IconImageRole]= "iconimage";
  names[IdRole]= "ident";
  return names;
}

QVariant FloorPlanItem::data(int role) const
{

  switch(role) {
  case InstallationRole:
    return installation();
  case DescriptionRole:
    return description();
  case PageRole:
    return page();
  case IconRole:
    return icon();
  case IdRole:
    return id();
  case FloorPlanImageRole:
    return floorplanImage();
  case IconImageRole:
    return iconImg();
  default:
    return QVariant();
  }
}
