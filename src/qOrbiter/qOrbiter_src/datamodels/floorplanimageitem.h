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

    This class is another specific model/item pair with the parent being parent being floorplan model.
    it is not however implmented directly as a model, but a QList<QOBject*>. This class holds very simple data pertaining to the
    images and descriptions of particular pages.

*/
#ifndef FLOORPLANITEM_H
#define FLOORPLANITEM_H


#include <QObject>
#include <QVariant>
#include <QImage>
#include <QMap>

class FloorPlanItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int m_page READ page NOTIFY pageChanged)
    Q_PROPERTY(QString m_description READ description NOTIFY descChanged)
    enum Roles {
        InstallationRole = Qt::UserRole+1,
        DescriptionRole =Qt::UserRole+2,
        PageRole= Qt::UserRole+3,
        PathRole = Qt::UserRole+4,
        FloorPlanImageRole = Qt::UserRole+5,
        IconImageRole = Qt::UserRole+6,
        IdRole = Qt::UserRole+7
    };

public:
    FloorPlanItem() {}
    explicit FloorPlanItem( QString &installation, QString &description,  int &page, QString imgPath,  QObject *parent);
    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;

    inline QString id() const {  return m_ident; }
    inline QString installation() const { return m_installation; }
    inline QString description() const { return m_description; }
    inline int page() const { return m_page; }
    inline QString icon() const { return m_iconPath; }
    inline QImage floorplanImage() const {  return m_floorplanImage; }
    inline QImage iconImg() const {  return m_iconImage; }
    inline QString imgPath() const {return m_imgPath;}

private:
    QString m_ident;
    QString m_installation;
    int m_page;
    QString m_description;
    QImage m_floorplanImage;
    QString m_iconPath;
    QImage m_iconImage;
    QString m_imgPath;

    QMap <int , QObject> devicelist ;

signals:
    void imageChanged();
    void dataChanged();
    void pageChanged();
    void descChanged();

};

#endif // FLOORPLANITEM_H
