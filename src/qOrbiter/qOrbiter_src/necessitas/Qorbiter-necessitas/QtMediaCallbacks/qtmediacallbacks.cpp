#include "qtmediacallbacks.h"
#include "org_kde_necessitas_origo_MediaCallbackInterface.h"
#include <jni.h>
#include <QDebug>

QtMediaCallbacks::QtMediaCallbacks(QObject *parent)
{

}

extern "C" { JNIEXPORT void JNICALL Java_org_kde_necessitas_origo_MediaCallbackInterface_setCurrentStatus
(JNIEnv *env, jobject obj, jstring str ){

    qDebug() << "Bridge function called";
}

JNIEXPORT jboolean JNICALL Java_org_kde_necessitas_origo_MediaCallbackInterface_androidPlaybackEnded
(JNIEnv *env, jobject obj, jboolean bl){

}

JNIEXPORT void JNICALL Java_org_kde_necessitas_origo_MediaCallbackInterface_setMediaPlaying
(JNIEnv *env, jobject obj, jboolean jbool){

//    if(jbool){
//        emit mediaStarted();
//    }
//    else{
//        emit mediaEnded();
//    }
}

JNIEXPORT void JNICALL Java_org_kde_necessitas_origo_MediaCallbackInterface_setMediaError
(JNIEnv *env, jobject obj, jstring str){


}

JNIEXPORT jint JNICALL Java_org_kde_necessitas_origo_MediaCallbackInterface_setMediaPosition
  (JNIEnv *env, jobject obj, jint i){

}

JNIEXPORT void JNICALL Java_org_kde_necessitas_origo_MediaCallbackInterface_setAndroidTotalTime
  (JNIEnv *env, jobject obj, jint i){
    int t = i;
   // emit totalTrackTimeChanged(t);
}

}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    qCritical("Starting Audio Service bridge::JNI_OnLoad()");
    JNIEnv* env;

    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        qCritical()<<"Can't get the enviroument";
        return -1;
    }


    qCritical("Starting Audio Service bridge::JNI_OnLoad()");
    return JNI_VERSION_1_6;
}
