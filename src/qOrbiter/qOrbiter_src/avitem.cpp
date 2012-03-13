/*
    This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org
   Langston Ball  golgoj4@gmail.com
    QOrbiter is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QOrbiter is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QOrbiter.  If not, see <http://www.gnu.org/licenses/>.

    AvItem.h/.cpp QObject based class for specific use with AV items and their associated data.
*/
#include "avitem.h"

AvItem::AvItem( QString &name, int &num,  int &templateNo, QImage &img, QString &attrib, QObject *parent) :
   m_name(name), m_deviceNo(num), m_deviceTemplate(templateNo), m_image(img), attribute(attrib)
{

}

QHash<int, QByteArray> AvItem::roleNames() const
{

  QHash<int, QByteArray> names;
  names[NameRole] = "name";
  names[NumberRole] = "deviceno";
  names[TemplateRole] = "devicetemplate";
  names[AttributeRole] = "attributes";
  names[ImageRole] = "icon";
  return names;
}

QVariant AvItem::data(int role) const
{
  switch(role) {
  case NameRole:
    return name();
  case NumberRole:
    return device_number();
  case ImageRole:
    return cellImage();
  case TemplateRole:
    return device_template();
   default:
    return QVariant();
  }
}
