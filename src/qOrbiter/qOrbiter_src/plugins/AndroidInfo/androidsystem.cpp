#include "androidsystem.h"
#include <QDebug>
#include <QApplication>

static JavaVM *m_pvm = 0;


static jclass buildVersionClass = 0;
static jclass buildVersionSDKClass = 0;

/* Cache of class ids and methodID lookups for use later for finding sdcard path. */
static jclass externalStorageClass =0;
static jclass fileClass = 0;
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
static jmethodID s_qtActivity_PlayMediaMethod=0;
static jmethodID s_qtActivity_StopMediaMethod=0;
static jmethodID s_qtActivity_SeekMediaMethod=0;
static jmethodID s_qtActivity_MediaControlMethod=0;




AndroidSystem::AndroidSystem(QObject *parent) :
    QObject(parent)
{

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
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    JNIEnv* env;

    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        qCritical()<<"Can't get the enviroument";
        return -1;
    }
    m_pvm = vm;
    s_qtactivity = (jclass)env->NewGlobalRef(env->FindClass("org/kde/necessitas/origo/QtActivity"));
    s_qtactivity_field = env->GetStaticMethodID(s_qtactivity, "getActivity", "()Lorg/kde/necessitas/origo/QtActivity;");
  //  s_qtActivity_PlayMediaMethod = env->GetMethodID(s_qtactivity, "playMedia", "(Ljava/lang/String;)V");
    s_qtActivity_MediaControlMethod = env->GetMethodID(s_qtactivity, "SendMediaCommand", "(Ljava/lang/String;IZLjava/lang/String;)Z");

    jclass localBuildClass = env->FindClass("android/os/Build");
    buildVersionClass = reinterpret_cast<jclass>(env->NewGlobalRef(localBuildClass));

    jclass localSDKclass = env->FindClass("android/os/Build$VERSION");
    buildVersionSDKClass =reinterpret_cast<jclass>(env->NewGlobalRef(localSDKclass));

    jclass lesc = env->FindClass("android/os/Environment");
    externalStorageClass = reinterpret_cast<jclass>(env->NewGlobalRef(lesc));

    jclass lfpid = env->FindClass("java/io/File");
    fileClass= reinterpret_cast<jclass>(env->NewGlobalRef(lfpid));

    findPathID=env->GetMethodID(fileClass, "getPath", "()Ljava/lang/String;");

    //    jclass localActivityClass = env->FindClass("android/app/NativeActivity");
    //    activityClass = reinterpret_cast<jclass>(env->NewGlobalRef(localActivityClass));
    //    getWindowManagerID = env->GetMethodID(localActivityClass, "getWindowManager", "()Landroid/view/WindowManager;");

    //    jclass localDisplayContextClassID= env->FindClass("android/content/Context");
    //   displayContextClass = reinterpret_cast<jclass>(env->NewGlobalRef(localDisplayContextClassID));
    //    contextMethodID = env->GetStaticMethodID(displayContextClass, "getSystemService", "()Ljava/lang/String;");

    //   jclass localDisplayObjClass = env->FindClass("android/view/Display;");
    // displayObjClass = reinterpret_cast<jclass>(env->NewGlobalRef(localDisplayObjClass));

    // displayID = env->GetMethodID(displayObjClass, "getDisplay", "I");


    //    jclass localWSclass =  env->FindClass("android/hardware/display/DisplayManager");
    //    window_service_class = reinterpret_cast<jclass>(env->NewGlobalRef(localWSclass));

    //    env->DeleteLocalRef(localDisplayContextClassID);
    //    env->DeleteLocalRef(localWSclass);
    //    env->DeleteLocalRef(localActivityClass);
    //    env->DeleteLocalRef(localDisplayObjClass);

    s_qtactivity = (jclass)env->NewGlobalRef(env->FindClass("org/kde/necessitas/origo/QtActivity"));
    s_qtactivity_field = env->GetStaticMethodID(s_qtactivity, "getActivity", "()Lorg/kde/necessitas/origo/QtActivity;");

    env->DeleteLocalRef(lesc);
    env->DeleteLocalRef(lfpid);

    //    env->DeleteLocalRef(localActivityClass);
    //   env->DeleteLocalRef(localDisplayContextClassID);
    //  env->DeleteLocalRef(localDisplayObjClass);
    qCritical() << "Exiting JNI onLoad";


    return JNI_VERSION_1_6;
}


bool AndroidSystem::findClassIdents()
{
    JNIEnv *env;
    if(m_pvm->AttachCurrentThread(&env, NULL)<0){
        qCritical()<<"AttachCurrentThread failed";
        return false;
    }

    m_qtActivity = env->NewGlobalRef(env->CallStaticObjectMethod(s_qtactivity, s_qtactivity_field));
    setStatusMessage("Device info Gather complete.");
    m_pvm->DetachCurrentThread();
    return true;
}

bool AndroidSystem::getDisplayInfo()
{
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
    return true;

}


bool AndroidSystem::updateExternalStorageLocation()
{

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
    return true;

}

bool AndroidSystem::updateBuildInformation()
{
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
    return true;

}

bool AndroidSystem::playMedia(QString url)
{
    JNIEnv* env;
    if (m_pvm->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";
        return false;
    }
    qWarning("Tryin media player");

    m_qtActivity = env->NewGlobalRef(env->CallStaticObjectMethod(s_qtactivity, s_qtactivity_field));

    if (!m_qtActivity){

        qWarning("Cant find activity!!");
        return false;
    }

    qWarning("Settin url.");

    QString str = url;
    int mSeek = 0;
    bool p = false;
    QString command = "play";

    jstring jstr = env->NewString(reinterpret_cast<const jchar*>(url.constData()), url.length());
    jstring jcom = env->NewString(reinterpret_cast<const jchar*>(command.constData()), command.length());

    jboolean res = env->CallBooleanMethod(m_qtActivity, s_qtActivity_MediaControlMethod,jcom, mSeek, p, jstr);
    env->DeleteLocalRef(jstr);
    env->DeleteLocalRef(jcom);
    m_pvm->DetachCurrentThread();
    return true;
}

bool AndroidSystem::stopMedia()
{
    JNIEnv* env;
    if (m_pvm->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";
        return false;
    }
    qWarning("Tryin media player");

    m_qtActivity = env->NewGlobalRef(env->CallStaticObjectMethod(s_qtactivity, s_qtactivity_field));

    if (!m_qtActivity){

        qWarning("Cant find activity!!");
        return false;
    }

    qWarning("Settin url.");



    qWarning("Calling bool JNI method.");
    QString str = "";
    int mSeek = 0;
    bool p = false;
    QString command = "stop";

    jstring jstr = env->NewString(reinterpret_cast<const jchar*>(str.constData()), str.length());
    jstring jcom = env->NewString(reinterpret_cast<const jchar*>(command.constData()), command.length());

    jboolean res = env->CallBooleanMethod(m_qtActivity, s_qtActivity_MediaControlMethod,jcom, mSeek, p, jstr);
    env->DeleteLocalRef(jstr);
    env->DeleteLocalRef(jcom);
    m_pvm->DetachCurrentThread();
    return true;
}

bool AndroidSystem::startAudioService()
{
    JNIEnv* env;
    if (m_pvm->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";
        return false;
    }

//    qCritical("Initializing Audio Service");
//    QString url = "http://fr.ahfm.com:9000";
//    jstring str = env->NewString(reinterpret_cast<const jchar*>(url.constData()), url.length());
//    jboolean res = env->CallBooleanMethod(m_qtActivity, s_serviceStart, str);

}
