#include "androidsystem.h"
#include <QDebug>
static JavaVM *m_pvm = 0;

/*Api Level*/
static jclass buildVersionClass=0;



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

    if(m_pvm)
    {
        setStatusMessage("Android Plugin Connected");
        findClassIdents();
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
    return JNI_VERSION_1_6;
}


void AndroidSystem::findClassIdents()
{
    JNIEnv *env;
    if(m_pvm->AttachCurrentThread(&env, NULL)<0){
        qCritical()<<"AttachCurrentThread failed";
    } else{
        buildVersionClass = env->FindClass("android/os/Build$VERSION");
        if(buildVersionClass){
            setStatusMessage("Located Build Version class");
            setApiLevel(env->GetStaticIntField(buildVersionClass, env->GetStaticFieldID(buildVersionClass,"SDK_INT", "I")));
            // qDebug() << env->get(buildVersionClass, env->GetStaticFieldID(buildVersionClass,"CODENAME", "()java/lang/String;"));
        }

        jclass buildClass = env->FindClass("android/os/Build");
        if(buildClass){
            qDebug() << "getting model info";
            jfieldID modelField =  env->GetStaticFieldID(buildClass, "MODEL","Ljava/lang/String;");
            jfieldID brandField = env->GetStaticFieldID(buildClass, "BRAND","Ljava/lang/String;");
            jfieldID manufacturerField = env->GetStaticFieldID(buildClass, "MANUFACTURER","Ljava/lang/String;");

            jstring modelString =(jstring)env->GetStaticObjectField(buildClass, modelField);
            jstring deviceString =(jstring)env->GetStaticObjectField(buildClass, brandField);
            jstring manufacturerString = (jstring) env->GetStaticObjectField(buildClass, manufacturerField);

            const char*  myConvertedModelString = env->GetStringUTFChars(modelString, 0);
            const char*  myConvertedBrandString = env->GetStringUTFChars(deviceString, 0);
            const char*  myConvertedManufacturerString = env->GetStringUTFChars(manufacturerString, 0);

            setDeviceBrand(QString::fromAscii(myConvertedBrandString));
            setDeviceManufacturer(QString::fromAscii(myConvertedManufacturerString));
            setDeviceName(QString::fromAscii(myConvertedModelString));
        }

        jclass externalStorageClass = env->FindClass("android.os.Environment");
        if(externalStorageClass){

            jmethodID storageMethodID = env->GetStaticMethodID(externalStorageClass, "getExternalStorageState", "()Ljava/lang/String;");
            jstring jStorageState = (jstring)env->CallObjectMethod(externalStorageClass, storageMethodID);
            const char* myConvertedStorageState = env->GetStringUTFChars(jStorageState, 0);
            if(strcmp(myConvertedStorageState, "mounted")==0){
                setMountStatus(true);

                 jmethodID storageLocationID = env->GetStaticMethodID(externalStorageClass, "getExternalStorageDirectory", "()Ljava/io/File;");
                 jobject myFileRef = env->CallStaticObjectMethod(externalStorageClass, storageLocationID);

                 jclass fileClass=env->FindClass("java/io/File");
                 jmethodID findPathID=env->GetMethodID(fileClass, "getPath", "()Ljava/lang/String;");

               jstring extPath = (jstring)env->CallObjectMethod(myFileRef, findPathID);

               const char*myPath = env->GetStringUTFChars(extPath, 0);
                setExternalStorageLocation(QString::fromAscii(myPath));
            }else
            {
                qDebug() << myConvertedStorageState;
                setMountStatus(false);
            }

        }

    }
    m_pvm->DetachCurrentThread();
    setStatusMessage("Device info Gather complete.");
}
