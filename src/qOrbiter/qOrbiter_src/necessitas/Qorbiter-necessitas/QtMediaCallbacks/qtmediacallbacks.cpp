#include "org_kde_necessitas_origo_MediaCallbackInterface.h"
#include <jni.h>
#include <QDebug>
#include <mediahandlers.h>


extern "C" {

//void mediaEnded();

//void mediaBegan();

//void mediaStopped();

//void setTotalTime(int i);


JNIEXPORT void JNICALL Java_org_kde_necessitas_origo_MediaCallbackInterface_setCurrentStatus
(JNIEnv *env, jobject obj, jstring str ){

    qDebug() << "JNI-LIB::setCurrentStatus(String)";
}

JNIEXPORT jboolean JNICALL Java_org_kde_necessitas_origo_MediaCallbackInterface_androidPlaybackEnded
(JNIEnv *env, jobject obj, jboolean bl){

      qDebug() << "JNI-LIB::playbackEnded(bool)";
}

JNIEXPORT void JNICALL Java_org_kde_necessitas_origo_MediaCallbackInterface_setMediaPlaying
(JNIEnv *env, jobject obj, jboolean jbool){

      qDebug() << "JNI-LIB::setMediaPlaying(bool)";

}

JNIEXPORT void JNICALL Java_org_kde_necessitas_origo_MediaCallbackInterface_setMediaError
(JNIEnv *env, jobject obj, jstring str){
      qDebug() << "JNI-LIB::setMediaError(String)";

}

JNIEXPORT jint JNICALL Java_org_kde_necessitas_origo_MediaCallbackInterface_setMediaPosition
(JNIEnv *env, jobject obj, jint i){
      qDebug() << "JNI-LIB::setMediaPosition(int)";
}

JNIEXPORT void JNICALL Java_org_kde_necessitas_origo_MediaCallbackInterface_setAndroidTotalTime
(JNIEnv *env, jobject obj, jint i){
    int tInt = i;
 qDebug() << "JNI-LIB::setAndroidTotalTime(int)";
    MediaHandlers *t = new MediaHandlers();
    qDebug() << "mediacallbacks::invoked::address" << &t;
    t->setAndroidTotalTime(tInt);
    t->squawk("Jni sent android time");

}
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    qCritical("Starting Audio Service bridge::JNI_OnLoad()");
    JNIEnv* env;

    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        qCritical()<<"Can't get the enviroument";
        return -1;

        //0x65639f98
        //0xbec3f598

    }


    qCritical("Starting Audio Service bridge::JNI_OnLoad()");
    return JNI_VERSION_1_6;
}
