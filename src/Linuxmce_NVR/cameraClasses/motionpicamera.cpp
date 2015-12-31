#include "motionpicamera.h"
#include "qnetworkaccessmanager.h"
#include "qnetworkrequest.h"
#include "qnetworkreply.h"
#include "qvariant.h"
#include "qurlquery.h"
#include "qeventloop.h"
#include "qbytearray.h"
#include "qimage.h"
#include "qdir.h"
#include "qthread.h"
#include "qdatetime.h"
#include "../managerClasses/eventlistener.h"
#include "abstractcameraevent.h"
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
const QString MotionPiCamera::CAMERA_CONFIG_GAP="gap";

const QString MotionPiCamera::ON_CAMERA_LOST="on_camera_lost";
const QString MotionPiCamera::ON_EVENT_END="on_event_end";
const QString MotionPiCamera::ON_EVENT_START="on_event_start";
const QString MotionPiCamera::ON_PICTURE_SAVE="on_picture_save";
const QString MotionPiCamera::ON_MOTION_DETECTED="on_motion_detected";
const QString MotionPiCamera::ON_MOVIE_END="on_movie_end";
const QString MotionPiCamera::ON_MOVIE_START="on_movie_start";

const QString MotionPiCamera::CURL_EVENT_START= "curl --data \"{'ip':'192.168.80.xxx','host': '%2','deviceId':%1,  'event': {'type': 'motion','status': 'started'} }\" http://%3:%4";
const QString MotionPiCamera::CURL_EVENT_END="curl --data \"{'ip':'192.168.80.xxx','host': '%2','deviceId':%1,'event': {'type': 'motion','status': 'stopped'} }\" http://%3:%4";
const QString MotionPiCamera::CURL_MOTION_DETECTED="curl --data \"{'ip':'192.168.80.xxx','host': '%2','deviceId':%1,'event': {'type': 'motion','status': 'detected'} }\" %3:%4";
const QString MotionPiCamera::CURL_SEND_PICTURE="curl --header \"X-dcedevice:%3\"  --form \"fileupload=@%f\" http://%1:%2 && rm %f";



MotionPiCamera::MotionPiCamera(QString cameraName, QString userName, QString password, quint16 port, quint16 control_port, QUrl url,  int dceId, QString camPath, QString callback_address, NvrCameraBase::CameraType t, NvrCameraBase::AudioType a, QObject *parent) :
    NvrCameraBase( cameraName, userName, password,port, control_port, url,camPath, dceId,  parent = 0) {

    setMotionState(0);
    int listenerPort =5100+dceId;
    setManagerPort(QString::number(listenerPort));
    setListener(new MotionEventListener( dceId, listenerPort));
    setManagerUrl(callback_address);


    QObject::connect(this,&MotionPiCamera::initialized, this, &MotionPiCamera::setConnections);
    QObject::connect(this, &MotionPiCamera::motionEnabledChanged, [=](){ sendDetectionCommand(getMotionEnabled() ? COMMAND_START : COMMAND_PAUSE); } );
    QObject::connect(this->getListener(), SIGNAL(motionEvent(CameraEvent*)), this, SLOT(handleMotionEvent(CameraEvent*)), Qt::QueuedConnection);
    setAudioType(a);
    setCameraType(t);

    if(constructed())
        initialized();

}

void MotionPiCamera::setConnections()
{
    httpManager = new QNetworkAccessManager();
    QObject::connect(httpManager, &QNetworkAccessManager::finished, this, &MotionPiCamera::handleControlReply);

    log( QString(Q_FUNC_INFO)+" motion camera setting connections"  );
    testControlPort();
    QString e = CURL_EVENT_END.arg(QString::number(dceDeviceId())).arg(cameraName()).arg(managerUrl()).arg(managerPort());
    e.replace("192.168.80.xxx", controlUrl().host());
    e.replace("SERVER_TARGET",managerUrl());
    e.replace("SERVER_PORT", managerPort());
    QVariant end(e.replace("HOSTNAME", cameraName()));

    QString s = CURL_EVENT_START.arg(QString::number(dceDeviceId())).arg(cameraName()).arg(managerUrl()).arg(managerPort());
    s.replace("192.168.80.xxx", controlUrl().host());
    s.replace("SERVER_TARGET",managerUrl());
    s.replace("SERVER_PORT", managerPort());
    QVariant start(s .replace("HOSTNAME", cameraName()));

    QString m = CURL_MOTION_DETECTED.arg(QString::number(dceDeviceId())).arg(cameraName()).arg(managerUrl()).arg(managerPort());
    m.replace("192.168.80.xxx", controlUrl().host());
    m.replace("SERVER_TARGET",managerUrl());
    m.replace("SERVER_PORT", managerPort());
    QVariant motion(m.replace("HOSTNAME", cameraName()));

    QString pic = CURL_SEND_PICTURE.arg(managerUrl()).arg(managerPort()).arg(QString::number(dceDeviceId()));


    setMotionSetting(MotionPiCamera::ON_EVENT_START, start );
    setMotionSetting(MotionPiCamera::ON_EVENT_END, end);
    setMotionSetting(MotionPiCamera::CAMERA_CONFIG_GAP, QVariant(1));
    setMotionSetting(MotionPiCamera::ON_MOTION_DETECTED, motion);
    setMotionSetting(MotionPiCamera::ON_PICTURE_SAVE, QVariant("rm -rf %f") );
    //setMotionSetting(MotionPiCamera::ON_PICTURE_SAVE, QVariant(pic));
    sendDetectionCommand(COMMAND_START);
}

void MotionPiCamera::testControlPort()
{

    QNetworkRequest req(controlUrl());
    httpManager->get(req);
}

void MotionPiCamera::handleControlReply(QNetworkReply *p)
{

    QString preParsed= p->readAll();

    if(preParsed.indexOf("<b>Done</b>")!=-1 || preParsed.indexOf(" write done !")!=-1 ){
        QRegExp r("set?.*.>");
        r.setMinimal(true);
        int i = r.indexIn(preParsed);

        QString setting;

        if(i!=-1){
            setting=r.cap(0);
        }

        QRegExp val(" = (.*.)</li>");
        val.setMinimal(true);

        if(preParsed.indexOf("write done")!=-1){

        } else {
            //  qDebug() << "Writing Settings " << setting.remove("set?").remove(">") << "::" << val.cap(1) <<"\n\n";
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
    QDateTime ti;
    QString fileName =QString("/tmp/"+QString::number(dceDeviceId())+"/lastsnap-%1.jpg").arg(ti.currentDateTime().currentMSecsSinceEpoch());
    QEventLoop *e = new QEventLoop();

    connect(rep, &QNetworkReply::finished, e, &QEventLoop::quit);
    connect(rep, &QNetworkReply::readyRead, [=](){      //lamda function for slot

        d_array.append(rep->readAll());

        int b = d_array.indexOf("--BoundaryString",0);
        int end = d_array.indexOf("--BoundaryString", b+1);
        int brk = d_array.indexOf("\xFF\xD8\xFF", 0);
        int brk2 = d_array.indexOf("\xFF\xD8\xFF", brk+1);

        if( (b!=-1 && end!=-1) && b!=end && brk !=-1){ //should have complete packet now

            if(brk!=-1){

                QImage t;
                if(t.loadFromData (d_array.mid(brk, (end-brk)) ) ){
                    qDebug() << t.height();
                    if(t.save(fileName, "JPEG")) {
                        log(" image saved ::" + fileName);
                        setCurrentFileName(fileName);
                        this->thread()->msleep(750);
                    }

                } else {
                    log("failed to save");
                }
                d_array.clear();
                rep->abort(); //disconnect now that we have an image
            }
        }
    } );
    e->exec(); //start event loop
}

void MotionPiCamera::handleMotionEvent(int device, bool detected)
{
    Q_UNUSED(device);

}

void MotionPiCamera::handleMotionEvent(CameraEvent *e)
{
    if(e->status()=="detected"){
        if(getMotionState()==2){
            return;
        } else {
            setMotionState(2);
        }
    } else if(e->status()=="started"){
        setMotionState(1);
    } else if(e->status()=="stopped"){
        setMotionState(0);
    }
    emit dispatchMotionEvent(e->deviceId(), e->sensorState()==CameraEvent::MOTION_DETECTED ? true : false);
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
    //refreshImage();
    QMetaObject::invokeMethod(this, "refreshImage", Qt::QueuedConnection);
    return getCurrentFileName().toStdString();

}

