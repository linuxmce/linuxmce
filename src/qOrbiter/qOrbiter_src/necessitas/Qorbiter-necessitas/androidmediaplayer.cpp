#include "androidmediaplayer.h"
#include <QDebug>

#include "androidmediaplayer.h"

static JavaVM* s_javaVM = 0;
//static jclass s_audioPlayerClassID = 0;
//static jmethodID s_audioPlayerConstructorMethodID=0;
//static jmethodID s_audioPlayerSetUrlMethodID=0;
//static jmethodID s_audioPlayerPlayMethodID=0;
//static jmethodID s_audioPlayerPauseMethodID=0;
//static jmethodID s_audioPlayerStopMethodID=0;
//static jmethodID s_audioPlayerReleaseMethodID=0;

//Used to obtain the reference to the QtActivity, via the crappy static
static jmethodID s_qtactivity_field =0;
static jclass s_qtactivity = 0;

//Used to actually do something useful
static jmethodID s_qtActivity_PlayMediaMethod=0;

androidMediaPlayer::androidMediaPlayer( QObject *parent) :
    QObject(parent)
{

    JNIEnv* env;

    // Qt is running in a different thread than Java UI, so you always Java VM *MUST* be attached to current thread
    if (s_javaVM->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";
        return;
    }

    m_qtActivity = env->NewGlobalRef(env->CallStaticObjectMethod(s_qtactivity, s_qtactivity_field));


    // Don't forget to detach from current thread
    s_javaVM->DetachCurrentThread();
}

androidMediaPlayer::~androidMediaPlayer()
{
    if (!m_qtActivity)
        return;

    JNIEnv* env;
    if (s_javaVM->AttachCurrentThread(&env, NULL)<0)
    {
        qCritical()<<"AttachCurrentThread failed";
        return;
    }

    s_javaVM->DetachCurrentThread();
}


bool androidMediaPlayer::playMedia(QString url)
{
    if (!m_qtActivity)
          return false;

      JNIEnv* env;
      if (s_javaVM->AttachCurrentThread(&env, NULL)<0)
      {
          qCritical()<<"AttachCurrentThread failed";
          return false;
      }

      jstring str = env->NewString(reinterpret_cast<const jchar*>(url.constData()), url.length());
      jboolean res = env->CallBooleanMethod(m_qtActivity, s_qtActivity_PlayMediaMethod, str);
      env->DeleteLocalRef(str);
      s_javaVM->DetachCurrentThread();
      return res;
}

//JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
//{
//    JNIEnv* env;
//    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
//        qCritical()<<"Can't get the enviroument";
//        return -1;
//    }

//    s_javaVM = vm;

//    s_qtactivity = (jclass)env->NewGlobalRef(env->FindClass("org/kde/necessitas/origo/QtActivity"));
//    s_qtactivity_field = env->GetStaticMethodID(s_qtactivity, "getActivity", "()Lorg/kde/necessitas/origo/QtActivity;");

//    // search for setUrl method
//    s_qtActivity_PlayMediaMethod = env->GetMethodID(s_qtactivity, "playMedia", "(Ljava/lang/String;)V");
//    if (!s_qtActivity_PlayMediaMethod)
//    {
//        qCritical()<<"Can't find playMedia method";
//        return -1;
//    }

//    return JNI_VERSION_1_6;
//}
