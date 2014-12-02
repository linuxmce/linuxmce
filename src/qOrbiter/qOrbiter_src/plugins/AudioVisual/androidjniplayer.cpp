#include "androidjniplayer.h"
#include "mediamanager.h"
#include "qdebug.h"
static JavaVM *m_pvm = 0;
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

AndroidJniPlayer::AndroidJniPlayer(MediaManager *controller, QObject *parent) :
    QObject(parent)
{
    mp_mediaManager=controller;
 qCritical() <<"AndroidJniPlay::JNI_OnLoad::constructor-end";
}

bool AndroidJniPlayer::initializeConnections()
{

}

//JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
//{
//    JNIEnv* env;
//    qCritical() <<"AndroidJniPlay::JNI_OnLoad::start";
//    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
//        qCritical()<<"Can't get the enviroument";
//        return -1;
//    }
//    m_pvm = vm;
//    s_qtactivity = (jclass)env->NewGlobalRef(env->FindClass("org/kde/necessitas/origo/QtActivity"));
//    s_qtActivity_StartMethod = env->GetMethodID(s_qtactivity, "startAudioService", "(J)V");
//    s_qtactivity_field = env->GetStaticMethodID(s_qtactivity, "getActivity", "()Lorg/kde/necessitas/origo/QtActivity;");
//    s_qtActivity_MediaControlMethod = env->GetMethodID(s_qtactivity, "SendMediaCommand", "(Ljava/lang/String;IZLjava/lang/String;F)Z");

//    /** New Media Commands */
//    s_qtActivity_ResumeMethod = env->GetMethodID(s_qtactivity, "resumeMedia", "()Z");
//    s_qtActivity_PauseMethod = env->GetMethodID(s_qtactivity, "pauseMedia", "()Z");
//    s_qtActivity_VolumeControlMethod = env->GetMethodID(s_qtactivity, "setVolumeLevel", "(F)Z");
//    s_qtActivity_SeekMediaMethod = env->GetMethodID(s_qtactivity, "seekToPosition", "(I)Z" );
//    s_qtActivity_StopMediaMethod = env->GetMethodID(s_qtactivity, "stopMedia", "()Z");
//    s_qtActivity_TimeCodeMethod = env->GetMethodID(s_qtactivity, "currentMediaPosition", "()I");


//    qCritical() <<"AndroidJniPlay::JNI_OnLoad::end";


//    return JNI_VERSION_1_6;
//}
