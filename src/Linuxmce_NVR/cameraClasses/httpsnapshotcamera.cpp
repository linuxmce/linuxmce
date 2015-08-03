#include "httpsnapshotcamera.h"
#include "qdebug.h"
#include "qnetworkaccessmanager.h"
#include "qnetworkrequest.h"
#include "qnetworkreply.h"
#include "qvariant.h"
#include "qurlquery.h"
#include "qeventloop.h"
#include "qbytearray.h"
#include "qimage.h"
#include "qdir.h"

HttpSnapshotCamera::HttpSnapshotCamera(QString cameraName, QString userName, QString password, quint16 port, quint16 control_port, QUrl url,  int dceId, QString camPath, NvrCameraBase::CameraType t, NvrCameraBase::AudioType a, QObject *parent) :
    NvrCameraBase( cameraName, userName, password,port, control_port, url, camPath, dceId,  parent = 0)
{
    setCameraType(t);
    setAudioType(a);

}

std::string HttpSnapshotCamera::getImage()
{

    QNetworkAccessManager *imgMgr= new QNetworkAccessManager();

    QNetworkRequest q(this->Url());
    q.url().setPath(getImagePath());
    q.url().setAuthority( QString("%1:%2").arg(userName()).arg( passWord() )  );

    QNetworkReply *rep = imgMgr->get(q);
    QString fileName ="/tmp/"+QString::number(dceDeviceId())+"/lastsnap.jpg";
    QEventLoop *e = new QEventLoop();

    connect(rep, &QNetworkReply::finished, e, &QEventLoop::quit);
    connect(rep, &QNetworkReply::readyRead, [=](){      //lamda function for slot

        mp_buffer.append(rep->readAll());
        int brk = mp_buffer.indexOf("\xFF\xD8\xFF");
           int len = rep->header(QNetworkRequest::ContentLengthHeader).toInt();
        if(brk!=-1 && mp_buffer.length() >= len ){
            QImage t;
            if(t.loadFromData(mp_buffer)){

                if(t.save(fileName, "JPEG"))
                    log(" image saved ::" + fileName);
                    mp_buffer.clear();
            } else {
                log("failed to save");
            }
        }


      //  qDebug() << mp_buffer;
/*
        int b = mp_buffer.indexOf("BoundaryString",0);
        int end = mp_buffer.indexOf("BoundaryString", b+1);


        if( (b!=-1 && end!=-1) && b!=end ){ //should have complete packet now

            if(brk!=-1){
                QByteArray imgData = mp_buffer.mid(brk, (brk-end));
                mp_buffer.remove(0,brk);
                QImage t;
                if(t.loadFromData(imgData)){

                    if(t.save(fileName, "JPEG"))
                        log(" image saved ::" + fileName);

                } else {
                    log("failed to save");
                }
                mp_buffer.clear();
              //  rep->abort(); //disconnect now that we have an image
            }
        }
        */
    } );
    e->exec(); //start event loop

    mp_buffer.clear();

    return fileName.toStdString();
}

bool HttpSnapshotCamera::checkMotionStatus()
{

    return false;
}
