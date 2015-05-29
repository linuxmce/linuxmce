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
    Q_PROPERTY (QString timestamp WRITE setTimeStamp READ getTimeStamp NOTIFY imageUpdated())

public:
    explicit SecurityVideoClass(QObject * qOrbiter_ptr, QObject *parent = 0);
    QMap<int, QImage> cameras; /**< The map of cameras */
    QTimer *requestInterval; /**< The requested interval. \warning do not set this too low, as it will slow down the router. */
    QImage currentFrame; /**< The current security image \warning depreciated */
    QString timestamp; /*!< \brief Access via securityVideo.timestamp. Returns QString time in HH:MM:SS:MMM \ingroup qorbiter_properties */

    /*!
     * \brief getTimeStamp getter for timestamp
     * \return
     */
    QString getTimeStamp ();

    /*!
     * \brief setTimeStamp setter for timestamp
     * \param t : timecode in QString format
     */
    void setTimeStamp(QString t);
signals:
    /*!
     * \brief This signal is updated when the security image changes.
     */
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
    void setCameraImage(int cam, QImage img);
    /*!
     * \brief getCameraImage - returns the camera image based on device id.
     * \param cam
     * \return
     *
     *Returns the security image from the map. If no image is found, an empty QImage is returned
     */
    QImage getCameraImage(int cam);

};

#endif // SECURITYVIDEOCLASS_H
