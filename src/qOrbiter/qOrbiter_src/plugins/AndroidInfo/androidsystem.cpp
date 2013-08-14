#include "androidsystem.h"
#include <QDebug>
#include <QApplication>

static JavaVM *m_pvm = 0;


jclass buildVersionClass;

static jclass externalStorageClass =0;
static jclass fileClass = 0;
//static jmethodID storageMethodID =0;
static jmethodID findPathID =0;
//static jmethodID storageLocationID=0 ;

jclass displayInfoClass;
jclass displayContextClass;
jclass activityClass;
jmethodID getWindowManagerID;


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
            setStatusMessage("Phase I complete.");
        }

        //        if(getDisplayInfo()){
        //            //  setStatusMessage("Phase 2 Complete");
        //        }

    }
    else
    {
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


    jclass lesc = env->FindClass("android.os.Environment");
    externalStorageClass = reinterpret_cast<jclass>(env->NewGlobalRef(lesc));

    jclass lfpid = env->FindClass("java/io/File");
    fileClass= reinterpret_cast<jclass>(env->NewGlobalRef(lfpid));
    findPathID=env->GetMethodID(fileClass, "getPath", "()Ljava/lang/String;");

    env->DeleteLocalRef(lesc);
    env->DeleteLocalRef(lfpid);


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

    qCritical("Storage Searching..");

    if(externalStorageClass!=0){
        qDebug() << "Found External Storage Class";
        jmethodID storageMethodID = env->GetStaticMethodID(externalStorageClass, "getExternalStorageState", "()Ljava/lang/String;");
        jmethodID storageLocationID = env->GetStaticMethodID(externalStorageClass, "getExternalStorageDirectory", "()Ljava/io/File;");

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

    setStatusMessage("Device info Gather complete.");
    return true;
}

bool AndroidSystem::getDisplayInfo()
{
    //    JNIEnv *env;
    //    if(m_pvm->AttachCurrentThread(&env, NULL)<0){
    //        qCritical()<<"AttachCurrentThread failed";
    //        return false;
    //    } else{
    //         displayContextClass = env->FindClass("android/content/Context");
    //        if(displayContextClass){
    //            setStatusMessage("Located Contexts");

    //             activityClass = env->FindClass("android/app/NativeActivity");

    //             getWindowManagerID = env->GetMethodID(activityClass, "getWindowManager", "()Landroid/view/WindowManager");


    //            jmethodID contextMethodID = env->GetMethodID(displayContextClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    //            jfieldID field_WINDOW_SERVICE = env->GetStaticFieldID(displayContextClass, "DISPLAY_SERVICE", "Ljava/lang/String;");
    //            jobject window_service_actual = env->CallObjectMethod( displayContextClass, contextMethodID, "display");

    //            jclass window_service_class = env->FindClass("android/hardware/display/DisplayManager");
    //            jmethodID getDisplaysID = env->GetMethodID(window_service_class, "getDisplays" , "()[Landroid/view/Display;" );
    //            jmethodID getNameID = env->GetMethodID(window_service_class, "toString", "()Ljava/lang/String;");
    //            jmethodID getDisplayID = env->GetMethodID(window_service_class, "getDisplay", "(I)Landroid/view/Display;");

    //            jobjectArray displayArrayObj = (jobjectArray)env->CallStaticObjectMethod(window_service_class, getDisplaysID);
    //             qDebug() << env->GetArrayLength(displayArrayObj);



    //              jobject display = env->GetObjectArrayElement(displayArrayObj, 0);
    //              jclass displayObjClass = env->FindClass("Landroid/view/Display;");
    //             jmethodID displayId = env->GetMethodID(displayObjClass, "getDisplay", "()I");
    //              qDebug() <<env->CallIntMethod(display, displayId);
    //        }

    //        return true;
    //    }
    return true;
}
