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
*/
#ifndef ABSTRACTIMAGEPROVIDER_H
#define ABSTRACTIMAGEPROVIDER_H

/*
  This class is intended to emcompass most of the image requests for qOrbiter. It has a reference to the
  qOrbitermanager, allowing for access to all of its class objects, include in the dce file request for
  remote loading of images and external. It has been decided that with a few exceptions, all listmodels will
  be responsible for managing their own image associations. This means that when any model activity occurs in the
  ui, the c++ model will be responsible for making the underlying changes to the model such as sorting,
  image association or other functions not defined at this time.
  */

#include <QObject>

#if (QT5)
#include <QtQuick/QQuickImageProvider>
#else
#include <QtDeclarative/QDeclarativeImageProvider>
#endif
#ifdef debug
#include <QDebug>
#endif
#include <QVariant>
#include <qorbitermanager.h>

#if (QT5)
class AbstractImageProvider: public QQuickImageProvider
#else
class AbstractImageProvider: public QDeclarativeImageProvider
#endif
{
public:
    AbstractImageProvider(qorbiterManager * manager );
    ~AbstractImageProvider();

    int widescreenAspect;
    int profileAspect;
    int tvAspect;
    int discAspect;


    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize)
    {
        QImage temp;
        QImage result;
        QSize returnSize=requestedSize;

        if (id.contains("floorplan"))
        {
             temp = floorplanProvider(returnSize);
       }
        else if(id.contains("securityimage"))
        {
           int camId = QString(id.section("/",-2,-2)).toInt();
            qDebug() << "Camera id" << camId;
            temp = securityProvider(returnSize, camId);
        }
        else if(id.contains("updateobject"))
        {
            temp = updateObjectProvider(returnSize);
        }
        else if(id.contains("screenshot"))
        {
            temp = screenShot(returnSize);
        }

        else if(id.contains("screensaver"))
        {
            temp = ScreenSaver();
        }
        else if(id.contains("stream"))
        {
            temp = Stream(returnSize);
        }

        //aspect checking


        if (requestedSize.isValid()) {
            result = temp.scaled(returnSize);
        } else {
            result= temp;
        }

        return result;


    }


signals:


public slots:


private:
    qorbiterManager * managerreference;
    QImage floorplanProvider(QSize &requestedSize) ;
    QImage securityProvider(QSize &requestedSize, int cam);
    QImage updateObjectProvider(QSize &requestedSize);
    QImage screenShot(QSize &requestedSize);
    QImage ScreenSaver();
    QImage Stream(QSize &requestedSize);

};

#endif // ABSTRACTIMAGEPROVIDER_H
