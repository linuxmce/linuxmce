#ifndef ABSTRACTCLOUDPROVIDER_H
#define ABSTRACTCLOUDPROVIDER_H

#include <QtPlugin>
#include "cloudsettings.h"

class QString;
class QObject;
class QSettings;
class CloudSettings;
/*!
 * \brief The CloudServiceInterface class. Abstract Base class for other cloud based and social media type classes to inherit from.
 *
 */
class CloudServiceInterface
{
public:
    virtual ~CloudServiceInterface(){}
    /*!
     * \brief deviceTemplate. Returns the device template for the given plugin as a method of identifying it within linuxmce.
     * \return
     */
    virtual int deviceTemplate() = 0;
    /*!
     * \brief setUserList. Set the users that have authenticated to this application
     */
    virtual void setUserList(QList<QObject*>) = 0;
    /*!
     * \brief suppliesPhotos. If this plugin supplies photos.
     * \return
     */
    virtual bool suppliesPhotos() = 0;
    /*!
     * \brief suppliesVideos. If this plugin provides videos.
     * \return
     */
    virtual bool suppliesVideos()=0;
    /*!
     * \brief suppliesContacts. If this plugin provides contacts.
     * \return
     */
    virtual bool suppliesContacts() = 0;

    /*!
     * \brief canSendEmail. If this plugin can send email
     * \return
     */
    virtual bool canSendEmail() = 0;
    /*!
     * \brief canSendFiles. If this plugin can store files off site.
     * \return
     */
    virtual bool canSendFiles() = 0;
    /*!
     * \brief canSendPhotos. If this plugin has a dedicated photo storage capability.
     * \return
     */
    virtual bool canSendPhotos() = 0;

    /*!
    * \brief name. The string representation of the name for this plugin. It is identified internally by deviceTemplate()
    * \return
    */
   virtual QString name() = 0;

    void setAccessToken(QString s){if(m_accessToken==s)return; m_accessToken=s; emit accessTokenChanged();}
    QString getAccessToken()const {return m_accessToken;}

signals:
    void authCodeChanged();
    void refreshTokenChanged();
    void accessTokenChanged();

protected:
    QString m_AuthCode;
    QString m_refreshToken;
    QString m_accessToken;
    CloudSettings * settingsManager;

};

QT_BEGIN_NAMESPACE

#define CloudServiceInterface_iid "CloudServiceInterface"
Q_DECLARE_INTERFACE(CloudServiceInterface, CloudServiceInterface_iid)

QT_END_NAMESPACE

#endif // ABSTRACTCLOUDPROVIDER_H
