/**
 *   Thanks google.
 *    https://code.google.com/p/razzlegames-android-ndk-tutorial/source/browse/trunk/jni/AndroidJNIHelper.h?r=4&spec=svn4
 *
 *    Create an easy to use way to grab jni environment
 *
 *    JNI method and constructor signature cheat sheet
 *
 *  B=byte
 *  C=char
 *  D=double
 *  F=float
 *  I=int
 *  J=long
 *  S=short
 *  V=void
 *  Z=boolean
 *  Lfully-qualified-class=fully qualified class
 *  [type=array of type>
 *  (argument types)return type=method type.
 *     If no arguments, use empty argument types: ().
 *     If return type is void (or constructor) use (argument types)V.*
 *
 *     Example
 *     @code
 *     constructor:
 *     (String s)
 *
 *     translates to:
 *     (Ljava/lang/String;)V
 *
 *     method:
 *     String toString()
 *
 *     translates to:
 *     ()Ljava/lang/String;
 *
 *     method:
 *     long myMethod(int n, String s, int[] arr)
 *
 *     translates to:
 *     (ILjava/lang/String;[I)J
 *     @endcode
 *
 */

#include "androidsystem.h"
#include <QDebug>
#include <QGuiApplication>
#include <QUrl>

static JavaVM *m_pvm = 0;

/** @defgroup Qt4JNIvariables Qt4 JNI methods and variables
 *  This group contains the method ids and classes to interface with android via the JNI to obtain information.
 *  Note that the Qt4 build uses necessitas and a more hands on method for accessing the JNI.
 *  @{
 *
 */
static jclass buildVersionClass = 0;
static jclass buildVersionSDKClass = 0;

static jclass externalStorageClass =0;
static jclass fileClass = 0;
static jclass linuxmceMediaService=0;
static jmethodID storageMethodID =0;
static jmethodID findPathID =0;
static jmethodID storageLocationID=0 ;

/* Cache of class id's and methodID lookups for use later for getting display information for the device */
static jclass displayInfoClass =0;
static jclass displayObjClass = 0;
static jclass displayContextClass= 0;
static jclass activityClass = 0;
static jclass window_service_class = 0;
static jmethodID getWindowManagerID=0;
static jmethodID contextMethodID =0;
static jmethodID displayID = 0;


/* Media Player */
//Used to obtain the reference to the QtActivity, via the crappy static
static jmethodID s_qtactivity_field =0;
static jclass s_qtactivity = 0;
static jmethodID s_qtActivity_ResumeMethod=0;
static jmethodID s_qtActivity_StartMethod=0;
static jmethodID s_qtActivity_PauseMethod=0;
static jmethodID s_qtActivity_TimeCodeMethod=0;
static jmethodID s_qtActivity_StopMediaMethod=0;
static jmethodID s_qtActivity_SeekMediaMethod=0;
static jmethodID s_qtActivity_MediaControlMethod=0;
static jmethodID s_qtActivity_VolumeControlMethod=0;


/** @defgroup Qt5JNIvariables Qt5 JNI methods and variables
 *  This group contains the QT5 method ids and classes to interface with android via the JNI to obtain information.
 *  Note that the Qt4 build uses necessitas and a more hands on method for accessing the JNI.
 *  @{
 *
 */


/** @} */

AndroidSystem::AndroidSystem(QObject *parent) :
    QObject(parent)
{
    setIsTablet(false);
    blueStandard = "#33B5E5";
    blueHighlight= "#0099CC";
    purpleStandard ="#AA66CC";
    purpleHighlight="#9933CC";
    greenStandard="#99CC00";
    greenHighlight="#669900";
    orangeStandard="#FFBB33";
    orangeHighlight="#FF8800";
    redStandard="#FF4444";
    redHighlight="#CC0000";
    externalStorageLocation = "";
    locationFunction = "getStorageLocation";
    lmceDataClass = "org/qtproject/qt5/android/bindings/SystemData";
    deviceDataClass ="android/os/Build";
#ifndef QT5
    if(m_pvm)
    {
        setStatusMessage("Android Plugin Connected");

        if(findClassIdents()){
            setStatusMessage("Found Class ID's.");
        }

        if(updateBuildInformation()){
            setStatusMessage("Retrieved build information");
        }

        setReadyStatus(true);
        qCritical("Initializing media player");

    }
    else
    {
        setReadyStatus(false);
        setStatusMessage("Android Plugin malfunction. Unable to get env.");
    }

#elif defined (QT5)
    bool activated;
    j_lmceData = new QAndroidJniObject(lmceDataClass.toLatin1());
    if(j_lmceData->isValid()){
        qDebug() << "Found System interface class.";
        activated = j_lmceData->callMethod<jboolean>("getActivated");
        qDebug() << " getActivated() " << activated;
    } else {
        activated=false;
        qCritical() << "No class " << lmceDataClass << " found!";
    }
    setReadyStatus(activated);

    deviceDataClass="android/os/Build";
    updateExternalStorageLocation();
    updateBuildInformation();

#endif
}

#ifndef QT5
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    JNIEnv* env;

    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        qCritical()<<"Can't get the enviroument";
        return -1;
    }
    m_pvm = vm;


    s_qtactivity = (jclass)env->NewGlobalRef(env->FindClass("org/kde/necessitas/origo/QtActivity"));
    linuxmceMediaService = (jclass)env->NewGlobalRef(env->FindClass("org/kde/necessitas/origo/LinuxmceAudioService"));
    s_qtActivity_StartMethod = env->GetMethodID(s_qtactivity, "startAudioService", "(J)V");
    s_qtactivity_field = env->GetStaticMethodID(s_qtactivity, "getActivity", "()Lorg/kde/necessitas/origo/QtActivity;");
    s_qtActivity_MediaControlMethod = env->GetMethodID(s_qtactivity, "SendMediaCommand", "(Ljava/lang/String;IZLjava/lang/String;F)Z");

    /** New Media Commands */
    s_qtActivity_ResumeMethod = env->GetMethodID(s_qtactivity, "resumeMedia", "()Z");
    s_qtActivity_PauseMethod = env->GetMethodID(s_qtactivity, "pauseMedia", "()Z");
    s_qtActivity_VolumeControlMethod = env->GetMethodID(s_qtactivity, "setVolumeLevel", "(F)Z");
    s_qtActivity_SeekMediaMethod = env->GetMethodID(s_qtactivity, "seekToPosition", "(I)Z" );
    s_qtActivity_StopMediaMethod = env->GetMethodID(s_qtactivity, "stopMedia", "()Z");
    s_qtActivity_TimeCodeMethod = env->GetMethodID(s_qtactivity, "currentMediaPosition", "()I");

    jclass localBuildClass = env->FindClass("android/os/Build");
    buildVersionClass = reinterpret_cast<jclass>(env->NewGlobalRef(localBuildClass));

    jclass localSDKclass = env->FindClass("android/os/Build$VERSION");
    buildVersionSDKClass =reinterpret_cast<jclass>(env->NewGlobalRef(localSDKclass));

    jclass lesc = env->FindClass("android/os/Environment");
    externalStorageClass = reinterpret_cast<jclass>(env->NewGlobalRef(lesc));

    jclass lfpid = env->FindClass("java/io/File");
    fileClass= reinterpret_cast<jclass>(env->NewGlobalRef(lfpid));

    findPathID=env->GetMethodID(fileClass, "getPath", "()Ljava/lang/String;");

    env->DeleteLocalRef(lesc);
    env->DeleteLocalRef(lfpid);

    //    env->DeleteLocalRef(localActivityClass);
    //   env->DeleteLocalRef(localDisplayContextClassID);
    //  env->DeleteLocalRef(localDisplayObjClass);
    qCritical() << "Exiting JNI onLoad";


    return JNI_VERSION_1_6;
}

#endif

bool AndroidSystem::findClassIdents()
{
#ifndef QT5
    JNIEnv *env;
    if(m_pvm->AttachCurrentThread(&env, NULL)<0){
        qCritical()<<"AttachCurrentThread failed";
        return false;
    }


    setStatusMessage("Device info Gather complete.");
    m_pvm->DetachCurrentThread();
#endif
    return true;
}

bool AndroidSystem::getDisplayInfo()
{
#ifndef QT5
    JNIEnv *env;
    if(m_pvm->AttachCurrentThread(&env, NULL)<0){
        qCritical()<<"AttachCurrentThread failed";
        return false;
    }

    //    if(displayContextClass!=0){
    //        setStatusMessage("Located Contexts");

    //        jfieldID field_WINDOW_SERVICE = env->GetStaticFieldID(displayContextClass, "DISPLAY_SERVICE", "Ljava/lang/String;");
    //        jobject window_service_actual = env->CallObjectMethod( displayContextClass, contextMethodID, "display");


    //        jmethodID getDisplaysID = env->GetMethodID(window_service_class, "getDisplays" , "()[Landroid/view/Display;" );
    //        jmethodID getNameID = env->GetMethodID(window_service_class, "toString", "()Ljava/lang/String;");
    //        jmethodID getDisplayID = env->GetMethodID(window_service_class, "getDisplay", "(I)Landroid/view/Display;");

    //        jobjectArray displayArrayObj = (jobjectArray)env->CallStaticObjectMethod(window_service_class, getDisplaysID);

    //        jobject display = env->GetObjectArrayElement(displayArrayObj, 0);

    //    }


    m_pvm->DetachCurrentThread();
#endif
    return true;

}


bool AndroidSystem::updateExternalStorageLocation()
{

#ifndef QT5
    JNIEnv *env;
    if(m_pvm->AttachCurrentThread(&env, NULL)<0){
        qCritical()<<"AttachCurrentThread failed";
        return false;
    }


    qCritical("Storage Searching..");

    if(externalStorageClass!=0){
        qDebug() << "Found External Storage Class";
        storageMethodID = env->GetStaticMethodID(externalStorageClass, "getExternalStorageState", "()Ljava/lang/String;");
        storageLocationID = env->GetStaticMethodID(externalStorageClass, "getExternalStorageDirectory", "()Ljava/io/File;");

        jstring jStorageState = (jstring)env->CallStaticObjectMethod(externalStorageClass, storageMethodID);

        const char* myConvertedStorageState = env->GetStringUTFChars(jStorageState, 0);

        env->ReleaseStringUTFChars(jStorageState, myConvertedStorageState);

        if(strcmp(myConvertedStorageState, "mounted")==0){
            qCritical("Media is mounted");
            setMountStatus(true);
            jobject myFileRef = env->CallStaticObjectMethod(externalStorageClass, storageLocationID);
            jstring extPath = (jstring)env->CallObjectMethod(myFileRef, findPathID);
            const char*myPath = env->GetStringUTFChars(extPath, 0);
            setExternalStorageLocation(QString::fromLatin1(myPath));
            env->ReleaseStringUTFChars(extPath, myPath);
            env->DeleteLocalRef(myFileRef);
            env->DeleteLocalRef(extPath);
        }else
        {
            qCritical("Media is not mounted!");
            setMountStatus(false);
        }

        env->DeleteLocalRef(jStorageState);

    }
    else{
        qCritical("externalStorageClass reference was invalid.");
    }

    qCritical("Storage Search Complete");
    m_pvm->DetachCurrentThread();
#elif defined (QT5)
    if(j_lmceData->isValid()){
        QAndroidJniObject exLoc = j_lmceData->callObjectMethod<jstring>( locationFunction.toLatin1());
        qDebug() << Q_FUNC_INFO << " - location " << exLoc.toString();
        setExternalStorageLocation(exLoc.toString());
    } else {
        qCritical() << Q_FUNC_INFO << " no class " << lmceDataClass << " found!";
        return false;
    }

#endif
    return true;

}

bool AndroidSystem::updateBuildInformation()
{
#ifndef QT5
    JNIEnv *env;
    if(m_pvm->AttachCurrentThread(&env, NULL)<0){
        qCritical()<<"AttachCurrentThread failed";
        return false;
    }
    setStatusMessage("Checking build info.");

    if(buildVersionClass !=0){

        jfieldID sdkTarget = env->GetStaticFieldID(buildVersionSDKClass, "SDK_INT", "I");
        jint apilvl = (jint)env->GetStaticIntField(buildVersionSDKClass, sdkTarget);

        setApiLevel(apilvl);


        jfieldID brand = env->GetStaticFieldID(buildVersionClass, "BRAND", "Ljava/lang/String;");
        jstring brand_txt =(jstring)env->GetStaticObjectField(buildVersionClass, brand);
        const char* myBrandTxt = env->GetStringUTFChars(brand_txt, 0);
        setDeviceBrand(QString::fromUtf8(myBrandTxt));
        env->ReleaseStringUTFChars(brand_txt, myBrandTxt);

        jfieldID device = env->GetStaticFieldID(buildVersionClass, "MODEL", "Ljava/lang/String;");
        jstring name_txt =(jstring)env->GetStaticObjectField(buildVersionClass, device);
        const char* myNameTxt = env->GetStringUTFChars(name_txt, 0);
        setDeviceName(QString::fromUtf8(myNameTxt));
        env->ReleaseStringUTFChars(name_txt, myNameTxt);
        setStatusMessage("Finished Gathering Build info.");

        jfieldID manuf = env->GetStaticFieldID(buildVersionClass, "MANUFACTURER", "Ljava/lang/String;");
        jstring manuf_txt =(jstring)env->GetStaticObjectField(buildVersionClass, manuf);
        const char* mymanufTxt = env->GetStringUTFChars(manuf_txt, 0);
        setDeviceManufacturer(QString::fromUtf8(mymanufTxt));
        env->ReleaseStringUTFChars(manuf_txt, mymanufTxt);
        setStatusMessage("Finished Gathering Build info.");

        env->DeleteLocalRef(manuf_txt);
        env->DeleteLocalRef(name_txt);
        env->DeleteLocalRef(brand_txt);

    }


    m_pvm->DetachCurrentThread();
#elif defined (QT5)

        int apilvl = QAndroidJniObject::getStaticField<jint>("android/os/Build$VERSION", "SDK_INT" );
        setApiLevel(apilvl);

        QString bd =QAndroidJniObject::getStaticObjectField<jstring>(deviceDataClass.toLatin1(), "BRAND").toString();
        setDeviceBrand(bd);

        QString dn = QAndroidJniObject::getStaticObjectField<jstring>(deviceDataClass.toLatin1(), "MODEL").toString();
        setDeviceName(dn);


#endif
    return true;

}

bool AndroidSystem::playMedia(QString url)
{
#ifndef QT5
    JNIEnv* env;
    if (m_pvm->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";
        return false;
    }
    m_qtActivity = env->NewGlobalRef(env->CallStaticObjectMethod(s_qtactivity, s_qtactivity_field));
    if (!m_qtActivity){
        qWarning("Cant find activity!!");
        return false;
    }

    QString str = url;
    if(url.length() < 4){
        qDebug() << url;
        return false;
    }
    int mSeek = 0;
    bool p = false;
    float f = 0.0;
    QString command = "play";
    qDebug() << url;
    jstring jstr = env->NewString(reinterpret_cast<const jchar*>(url.constData()), url.length());
    jstring jcom = env->NewString(reinterpret_cast<const jchar*>(command.constData()), command.length());
    jboolean res = env->CallBooleanMethod(m_qtActivity, s_qtActivity_MediaControlMethod,jcom, mSeek, p, jstr, f);
    env->DeleteLocalRef(jstr);
    env->DeleteLocalRef(jcom);
    m_pvm->DetachCurrentThread();
    return true;
#else
    return false;
#endif

}

bool AndroidSystem::stopMedia()
{
#ifndef QT5
    JNIEnv* env;
    if (m_pvm->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";
        return false;
    }

    m_qtActivity = env->NewGlobalRef(env->CallStaticObjectMethod(s_qtactivity, s_qtactivity_field));

    if (!m_qtActivity){

        qWarning("Cant find activity!!");
        return false;
    }

    QString str = "";
    int mSeek = 0;
    bool p = false;
    QString command = "stop";

    jstring jstr = env->NewString(reinterpret_cast<const jchar*>(str.constData()), str.length());
    jstring jcom = env->NewString(reinterpret_cast<const jchar*>(command.constData()), command.length());
    float f=0.0;
    jboolean res = env->CallBooleanMethod(m_qtActivity, s_qtActivity_MediaControlMethod,jcom, mSeek, p, jstr, f);
    env->DeleteLocalRef(jstr);
    env->DeleteLocalRef(jcom);
    m_pvm->DetachCurrentThread();
    return true;
#else
    return false;
#endif
}

bool AndroidSystem::startAudioService(long addr)
{
#ifndef QT5
    JNIEnv* env;
    if (m_pvm->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";
        return false;
    }
    qCritical("Initializing Audio Service");
    jint cbAddr = addr;
    m_qtActivity = env->NewGlobalRef(env->CallStaticObjectMethod(s_qtactivity, s_qtactivity_field, cbAddr));
    env->CallBooleanMethod(m_qtActivity, s_qtActivity_StartMethod);
    return true;
#else
    return false;
#endif

}

bool AndroidSystem::setVolume(double vol)
{

#ifndef QT5
    JNIEnv* env;
    if (m_pvm->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";
        return false;
    }

    m_qtActivity = env->NewGlobalRef(env->CallStaticObjectMethod(s_qtactivity, s_qtactivity_field));

    if (!m_qtActivity){

        qWarning("Cant find activity!!");
        return false;
    }
    float f = float(vol);
    QString str = "";
    int mSeek = 0;
    bool p = false;
    QString command = "setVolume";

    jstring jstr = env->NewString(reinterpret_cast<const jchar*>(str.constData()), str.length());
    jstring jcom = env->NewString(reinterpret_cast<const jchar*>(command.constData()), command.length());

    jboolean res = env->CallBooleanMethod(m_qtActivity, s_qtActivity_MediaControlMethod,jcom, mSeek, p, jstr, f);
    env->DeleteLocalRef(jstr);
    env->DeleteLocalRef(jcom);
    m_pvm->DetachCurrentThread();
    return true;
#else
    return false;
#endif
}

void AndroidSystem::mediaStop()
{
#ifndef QT5
    JNIEnv* env;
    if (m_pvm->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";
        return ;
    }

    m_qtActivity = env->NewGlobalRef(env->CallStaticObjectMethod(s_qtactivity, s_qtactivity_field));

    if (!m_qtActivity){

        qWarning("Cant find activity!!");
        return;
    }

    jboolean res = env->CallBooleanMethod(m_qtActivity, s_qtActivity_StopMediaMethod);
    m_pvm->DetachCurrentThread();

#endif
}

void AndroidSystem::mediaPlay()
{

}

void AndroidSystem::mediaPause()
{
#ifndef QT5
    JNIEnv* env;
    if (m_pvm->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";
        return ;
    }

    m_qtActivity = env->NewGlobalRef(env->CallStaticObjectMethod(s_qtactivity, s_qtactivity_field));

    if (!m_qtActivity){

        qWarning("Cant find activity!!");
        return;
    }

    jboolean res = env->CallBooleanMethod(m_qtActivity, s_qtActivity_PauseMethod);
    m_pvm->DetachCurrentThread();

#endif
}

int AndroidSystem::getMediaPosition()
{
#ifndef QT5
    JNIEnv* env;
    if (m_pvm->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";
        return 0 ;
    }

    m_qtActivity = env->NewGlobalRef(env->CallStaticObjectMethod(s_qtactivity, s_qtactivity_field));

    if (!m_qtActivity){

        qWarning("Cant find activity!!");
        return 0;
    }

    jint res = env->CallIntMethod(m_qtActivity, s_qtActivity_TimeCodeMethod);
    int rt = res;

    m_pvm->DetachCurrentThread();
    return rt;
#endif
}

void AndroidSystem::mediaResume()
{
#ifndef QT5
    JNIEnv* env;
    if (m_pvm->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";
        return ;
    }

    m_qtActivity = env->NewGlobalRef(env->CallStaticObjectMethod(s_qtactivity, s_qtactivity_field));

    if (!m_qtActivity){

        qWarning("Cant find activity!!");
        return;
    }

    jboolean res = env->CallBooleanMethod(m_qtActivity, s_qtActivity_ResumeMethod);
    m_pvm->DetachCurrentThread();

#endif

}

void AndroidSystem::mediaSeek(int s)
{
#ifndef QT5
    JNIEnv* env;
    if (m_pvm->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";
        return ;
    }

    m_qtActivity = env->NewGlobalRef(env->CallStaticObjectMethod(s_qtactivity, s_qtactivity_field));

    if (!m_qtActivity){

        qWarning("Cant find activity!!");
        return;
    }

    jboolean res = env->CallBooleanMethod(m_qtActivity, s_qtActivity_SeekMediaMethod, s );
    m_pvm->DetachCurrentThread();

#endif
}

void AndroidSystem::mediaSetVol(double vol)
{
#ifndef QT5
    JNIEnv* env;
    if (m_pvm->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";
        return ;
    }

    m_qtActivity = env->NewGlobalRef(env->CallStaticObjectMethod(s_qtactivity, s_qtactivity_field));

    if (!m_qtActivity){

        qWarning("Cant find activity!!");
        return;
    }

    float lvl = float(vol);
    jboolean res = env->CallBooleanMethod(m_qtActivity, s_qtActivity_VolumeControlMethod, lvl );
    m_pvm->DetachCurrentThread();

#endif
}
