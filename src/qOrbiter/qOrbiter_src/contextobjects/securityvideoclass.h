#ifndef SECURITYVIDEOCLASS_H
#define SECURITYVIDEOCLASS_H

#include <QObject>
#include <QMap>
#include <QTimer>
#include <QImage>
#include <QTime>
#include <QImage>

/*!
 *
 *\brief This is a c++ based class with the job of keeping track of security cameras and providing timestamps.
 *
 *\ingroup context_objects
 *
 *The securityvideoclass is responsible for managing the security camera views. The different camera id's are stored in a map for quick reference.
 *When a security picture is asked for, the AbstractImageProvider processes the url and returns the image based on which camera id is in the url.
 *The other purpose of this class is to provide timestamps for other image providers due to the nature of the AbstractImageProvider. By using
 *timestamps, we can request new data by changing the image 'url'.
 *
 *
 */
class SecurityVideoClass : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QString timestamp WRITE setTimeStamp READ getTimeStamp NOTIFY imageUpdated()) /*! Access via securityVideo.timestamp \return QString time in HH:MM:SS:MMM \ingroup security_commands */

public:
    explicit SecurityVideoClass(QObject *parent = 0);
    QMap<int, QImage> cameras; /**< The map of cameras */
    QTimer *requestInterval; /**< The requested interval. \warning do not set this too low, as it will slow down the router. */
    QImage currentFrame; /**< The current security image \warning depreciated */
    QString timestamp; /**< The current timestamp \relates getTimeStamp() \relatesalso setTimeStamp */

    /*!
     * \brief getTimeStamp getter for timestamp
     * \return
     */
    QString getTimeStamp () {return QTime::currentTime().toString(); }

    /*!
     * \brief setTimeStamp setter for timestamp
     * \param t : timecode in QString format
     */
    void setTimeStamp(QString t) {timestamp = QTime::currentTime().toString(); emit imageUpdated();}

signals:
    void imageUpdated();

public slots:
    /*!
     * \brief setCameraImage set the camera image based on the device id passed to it.
     * \param cam
     * \param img
     *If cam (the camera device id) is in the map, the image is set to that position. If not, a new key with the device id is created and img
     *is inserted into the map.
     *
     * \todo delete image after camera view closes.
     */
    void setCameraImage(int cam, QImage img) {
        if(cameras.find(cam).key()==cam)
        {
            cameras.find(cam).value() = img;
        }
        else{
            cameras.insert(cam, img);
        }
        currentFrame = img;
        emit imageUpdated();
    }
    /*!
     * \brief getCameraImage - returns the camera image based on device id.
     * \param cam
     * \return
     *
     *Returns the security image from the map. If no image is found, an empty QImage is returned
     */
    QImage getCameraImage(int cam){
        if(cameras.find(cam).key()==cam)
        {
            return cameras.find(cam).value();
        }
        else{
           return QImage();
        }

    }


};

#endif // SECURITYVIDEOCLASS_H
