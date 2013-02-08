#include "floorplanimageitem.h"

FloorPlanItem::FloorPlanItem(QString &installation, QString &description,  int &page,   QString imgPath, QObject *parent) :
    m_installation(installation), m_description(description), m_page(page), m_imgPath(imgPath)
{
    m_ident = installation + "-" + QString::number(page) ;

}

QHash<int, QByteArray> FloorPlanItem::roleNames() const
{

  QHash<int, QByteArray> names;
  names[InstallationRole] = "installation";
  names[DescriptionRole] = "description";
  names[PageRole] = "page";
  names[PathRole] = "imgpath";
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
  case PathRole:
    return imgPath();
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
