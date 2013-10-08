#include "org_kde_necessitas_origo_CallbackInterface.h"

JNIEXPORT void JNICALL Java_org_kde_necessitas_origo_CallbackInterface_setMediaPlaying
  (JNIEnv *, jobject, jboolean){

}

JNIEXPORT void JNICALL Java_org_kde_necessitas_origo_CallbackInterface_setMediaError
  (JNIEnv *, jobject, jstring){

}

JNIEXPORT jboolean JNICALL Java_org_kde_necessitas_origo_CallbackInterface_androidPlaybackEnded
  (JNIEnv *, jobject, jboolean){

}

JNIEXPORT jint JNICALL Java_org_kde_necessitas_origo_CallbackInterface_setMediaPosition
  (JNIEnv *, jobject, jint){

}


JNIEXPORT void JNICALL Java_org_kde_necessitas_origo_CallbackInterface_setAndroidTotalTime
  (JNIEnv *, jobject, jint);
