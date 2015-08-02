#include "motionpicamera.h"
#include <QNetworkAccessManager>
#include "qnetworkrequest.h"
#include "qnetworkreply.h"
#include "qvariant.h"
#include "qurlquery.h"
#include "qeventloop.h"
#include "qbytearray.h"
#include "qimage.h"
#include "qdir.h"
const QString MotionPiCamera::WRITE_SETTINGS="/0/config/writeyes";
const QString MotionPiCamera::ALL_CAMERAS="/0";
const QString MotionPiCamera::CAMERA_DETECTION_START="/0/detection/start";
const QString MotionPiCamera::CAMERA_DETECTION_STATUS="/0/detection/status";
const QString MotionPiCamera::CAMERA_DETECTION_CONNECTION="/0/detection/connection";
const QString MotionPiCamera::CAMERA_DETECTION_PAUSE="/0/detection/pause";


const QString MotionPiCamera::CAMERA_CONFIG_HEIGHT="height";
const QString MotionPiCamera::CAMERA_CONFIG_WIDTH="width";
const QString MotionPiCamera::CAMERA_CONFIG_TARGET_DIR="target_dir";
const QString MotionPiCamera::CAMERA_CONFIG_SNAPSHOT_FILENAME="snapshot_filename";
const QString MotionPiCamera::CAMERA_CONFIG_JPG_FILENAME="jpeg_filename";
const QString MotionPiCamera::CAMERA_CONFIG_TIMELAPSE_FILENAME="timelapse_filename";
const QString MotionPiCamera::CAMERA_CONFIG_LOCATE="locate";
const QString MotionPiCamera::CAMERA_CONFIG_ROTATE="rotate";

const QString MotionPiCamera::ON_CAMERA_LOST="on_camera_lost";
const QString MotionPiCamera::ON_EVENT_END="on_event_end";
const QString MotionPiCamera::ON_EVENT_START="on_event_start";
const QString MotionPiCamera::ON_PICTURE_SAVE="on_picture_save";
const QString MotionPiCamera::ON_MOTION_DETECTED="on_motion_detected";
const QString MotionPiCamera::ON_MOVIE_END="on_movie_end";
const QString MotionPiCamera::ON_MOVIE_START="on_movie_start";

const QString MotionPiCamera::CURL_EVENT_START= "curl --data \"{'ip':'192.168.80.xxx','host': 'HOSTNAME','event': {'type': 'motion','status': 'started'} }\" http://SERVER_TARGET:SERVER_PORT";
const QString MotionPiCamera::CURL_EVENT_END="curl --data \"{'ip':'192.168.80.xxx','host': 'HOSTNAME','event': {'type': 'motion','status': 'stopped'} }\" http://SERVER_TARGET:SERVER_PORT";
const QString MotionPiCamera::CURL_MOTION_DETECTED="curl --data \"{'ip':'192.168.80.xxx','host': 'HOSTNAME','event': {'type': 'motion','status': 'detected'} }\" SERVER_TARGET:SERVER_PORT";
const QString MotionPiCamera::CURL_SEND_PICTURE="curl  --form \"fileupload=@%f\" http://SERVER_TARGET:SERVER_PORT";


MotionPiCamera::MotionPiCamera(QString cameraName, QString userName, QString password, quint16 port, quint16 control_port, QUrl url,  int dceId, NvrCameraBase::CameraType t, NvrCameraBase::AudioType a, QObject *parent) :
    NvrCameraBase( cameraName, userName, password,port, control_port, url, dceId, parent = 0) {

    QObject::connect(this,&MotionPiCamera::initialized, this, &MotionPiCamera::setConnections);
    QObject::connect(this, &MotionPiCamera::motionEnabledChanged, [=](){ sendDetectionCommand(getMotionEnabled() ? COMMAND_START : COMMAND_PAUSE); } );

    setAudioType(a);
    setCameraType(t);

    //need to set this up in dce
    QDir out("/tmp/"+QString::number(dceId)+"/");

    if(!out.exists()){

        if(out.mkpath(out.path()))
            log("Output path created");

    }



    if(constructed())
        initialized();

}

void MotionPiCamera::setConnections()
{
    httpManager = new QNetworkAccessManager();
    QObject::connect(httpManager, &QNetworkAccessManager::finished, this, &MotionPiCamera::handleControlReply);

    log( QString(Q_FUNC_INFO)+" motion camera setting connections"  );
    testControlPort();
    QString e = CURL_EVENT_END;
    e.replace("192.168.80.xxx", controlUrl().host());
    e.replace("SERVER_TARGET",managerUrl());
    e.replace("SERVER_PORT", managerPort());
    QVariant end(e.replace("HOSTNAME", cameraName()));

    QString s = CURL_EVENT_START;
    s.replace("192.168.80.xxx", controlUrl().host());
    s.replace("SERVER_TARGET",managerUrl());
    s.replace("SERVER_PORT", managerPort());
    QVariant start(s .replace("HOSTNAME", cameraName()));

    QString m = CURL_MOTION_DETECTED;
    m.replace("192.168.80.xxx", controlUrl().host());
    m.replace("SERVER_TARGET",managerUrl());
    m.replace("SERVER_PORT", managerPort());
    QVariant motion(m.replace("HOSTNAME", cameraName()));

    QString pic = CURL_SEND_PICTURE;
    pic.replace("SERVER_TARGET",managerUrl());
    pic.replace("SERVER_PORT", managerPort());

    setMotionSetting(MotionPiCamera::ON_EVENT_START, start );
    setMotionSetting(MotionPiCamera::ON_EVENT_END, end);
    setMotionSetting(MotionPiCamera::ON_MOTION_DETECTED, motion);
    setMotionSetting(MotionPiCamera::ON_PICTURE_SAVE, QVariant(pic));
    sendDetectionCommand(COMMAND_PAUSE);
}

void MotionPiCamera::testControlPort()
{

    QNetworkRequest req(controlUrl());
    httpManager->get(req);
}

void MotionPiCamera::handleControlReply(QNetworkReply *p)
{

    log(Q_FUNC_INFO);

    QString preParsed= p->readAll();

    if(preParsed.indexOf("<b>Done</b>")!=-1 || preParsed.indexOf(" write done !")!=-1 ){
        QRegExp r("set?.*.>");
        r.setMinimal(true);
        int i = r.indexIn(preParsed);

        QString setting;

        if(i!=-1){
            setting=r.cap(0);
            //   qDebug() << "setting::" << setting.remove("set?").remove(">") ;
        }

        QRegExp val(" = (.*.)</li>");
        val.setMinimal(true);
        int valCheck = val.indexIn(preParsed);
        //   qDebug() << "value::" << val.cap(1)<< "\n";

        if(preParsed.indexOf("write done")!=-1){

        } else {
            qDebug() << "Writing Settings " << setting.remove("set?").remove(">") << "::" << val.cap(1) <<"\n\n";
            writeSettings();
        }
    }

    p->deleteLater();

}

void MotionPiCamera::parseStream(QByteArray st)
{

}

void MotionPiCamera::refreshImage()
{

    QNetworkAccessManager *imgMgr= new QNetworkAccessManager();

    QNetworkRequest q(this->Url());

    QNetworkReply *rep = imgMgr->get(q);
    QString fileName ="/tmp/"+QString::number(dceDeviceId())+"/lastsnap.jpg";
    QEventLoop *e = new QEventLoop();

    connect(rep, &QNetworkReply::finished, e, &QEventLoop::quit);
    connect(rep, &QNetworkReply::readyRead, [=](){      //lamda function for slot

        d_array.append(rep->readAll());

        int b = d_array.indexOf("BoundaryString",0);
        int end = d_array.indexOf("BoundaryString", b+1);
        int brk = d_array.indexOf("\xFF\xD8\xFF");

        if( (b!=-1 && end!=-1) && b!=end ){ //should have complete packet now

            if(brk!=-1){
                QByteArray imgData = d_array.mid(brk, (brk-end));
                d_array.remove(0,brk);
                QImage t;
                if(t.loadFromData(imgData)){

                    if(t.save(fileName, "JPEG"))
                        log(" image saved ::" + fileName);

                } else {
                    log("failed to save");
                }
                d_array.clear();
                rep->abort(); //disconnect now that we have an image
            }
        }
    } );
    e->exec(); //start event loop

    d_array.clear();
}

void MotionPiCamera::setCameraType(NvrCameraBase::CameraType t)
{
    if(m_cameraType==t)
        return;

    m_cameraType= t;
    emit cameraTypeChanged();
}

void MotionPiCamera::setAudioType(NvrCameraBase::AudioType a)
{
    if(m_audioType == a)
        return;

    m_audioType = a;
    emit audioTypeChanged();
}

void MotionPiCamera::sendDetectionCommand(MotionPiCamera::DetectionCommand d)
{
    QUrl cmdUrl(controlUrl());

    switch (d) {
    case MotionPiCamera::COMMAND_PAUSE:
        cmdUrl.setPath(CAMERA_DETECTION_PAUSE);
        break;
    case MotionPiCamera::COMMAND_START:
        cmdUrl.setPath(CAMERA_DETECTION_START);
        break;
    case MotionPiCamera::COMMAND_STATUS:
        cmdUrl.setPath(CAMERA_DETECTION_STATUS);
        break;
    case MotionPiCamera::COMMAND_CONNECTION:
        cmdUrl.setPath(CAMERA_DETECTION_CONNECTION);
        break;
    default:
        break;
    }
    QNetworkRequest req(cmdUrl);
    httpManager->get(req);
}

void MotionPiCamera::getMotionSetting(QString s)
{
    QUrl settingUrl(controlUrl());
    settingUrl.setPath("/0/config/get");
    settingUrl.setQuery("query="+s);
    QNetworkRequest req(settingUrl);
    httpManager->get(req);
}

void MotionPiCamera::setMotionSetting(QString Setting, QVariant val)
{
    QString pt =controlUrl().toString();
    pt.append("0/config/set?"+Setting+"="+val.toString());
    QUrl test(pt);
    QNetworkRequest req(test);
    httpManager->get(req);
}

void MotionPiCamera::writeSettings()
{
    QUrl writeUrl(controlUrl());
    writeUrl.setPath(WRITE_SETTINGS);
    QNetworkRequest req(writeUrl);
    httpManager->get(req);

}

std::string MotionPiCamera::getImage()
{
    qDebug() << Q_FUNC_INFO;

    refreshImage();
    return QString("/tmp/"+QString::number(dceDeviceId())+"/lastsnap.jpg").toStdString();

}

