/*
    This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org
    Author: Langston Ball
   Langston Ball  golgoj4@gmail.com
    QOrbiter is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QOrbiter is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QOrbiter.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "androidvideosurface.h"
#include <QGLWidget>
#include <QTimer>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <jni.h>
#include <android/log.h>

/* Definitions */
#define ATTACH_TO_CURRENT_THREAD_THEN_RET(ret_statement)     \
   JNIEnv* env;                                              \
   if (m_javaVM->AttachCurrentThread(&env, NULL) < 0) {      \
       qCritical() << "Failed to attach to current thread."; \
       ret_statement;                                        \
   }

#define DETACH_FROM_CURRENT_THREAD_THEN_RET(ret_statement)       \
   if (!m_javaVM->DetachCurrentThread() < 0) {                   \
       qCritical() << "Failed to deattach from current thread."; \
       ret_statement;                                            \
   }

/* Static jni variables */
static JavaVM* m_javaVM;
static JNIEnv* m_env;
static jmethodID mid_loadVideoTexture;
static jmethodID mid_updateTexture;
static jclass class_TextureHelper;
static jclass class_SurfaceTexture;

/*!
 * \brief load_custom_java_classes. Attempts to load the custom java classes that operate on the other side of this class
 * so to speak that deal with interfacing with the media player. Classnames are hardcoded.
 * \param env
 * \return
 */
int load_custom_java_classes(JNIEnv* env)
{
    const char* classNameTexHelper =   "org/kde/necessitas/origo/TextureHelper";
    const char* classNameSurfTexture = "org/kde/necessitas/origo/MySurfaceTexture";

    // TODO: remember to free.
    jclass local_class_java_delegate = env->FindClass(classNameTexHelper);
    jclass local_class_surf_texture  = env->FindClass(classNameSurfTexture);
    if (!local_class_java_delegate) {
        __android_log_print(ANDROID_LOG_FATAL, "Qt", "Unable to find class %s.", classNameTexHelper);
        return JNI_FALSE;
    }
    if (!local_class_surf_texture) {
       __android_log_print(ANDROID_LOG_FATAL, "Qt", "Unable to find class %s.", classNameSurfTexture);
       return JNI_FALSE;
    }

    class_TextureHelper  = (jclass)env->NewGlobalRef(local_class_java_delegate);
    class_SurfaceTexture = (jclass)env->NewGlobalRef(local_class_surf_texture);
    env->DeleteLocalRef(local_class_java_delegate);
    env->DeleteLocalRef(local_class_surf_texture);

    mid_loadVideoTexture = env->GetStaticMethodID(class_TextureHelper,
                                                  "loadVideoTexture",
                                                  "(I)Landroid/graphics/SurfaceTexture;");
    mid_updateTexture    = env->GetMethodID(class_SurfaceTexture,
                                            "updateTexImage",
                                            "()V");
    if (!mid_updateTexture)
       exit(-1);

    return JNI_TRUE;
}

/*! \brief. JNI_OnLoad is called when first initialized, allowing it to ascertain certain environment variables.
 *
 */
Q_DECL_EXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
   qDebug("JNI_OnLoad invoked!");
   m_javaVM = vm;

   if (m_javaVM->GetEnv((void**)&m_env, JNI_VERSION_1_4) != JNI_OK) {
      __android_log_print(ANDROID_LOG_FATAL, "Qt", "GetEnv failed.");
      return JNI_ERR;
   }

   // lcarlon: load the classes for the Qt application here.
   if (!load_custom_java_classes(m_env)) {
      __android_log_print(ANDROID_LOG_FATAL, "Qt", "Couldn't register user defined classes.");
      return JNI_ERR;
   }

   return JNI_VERSION_1_4;
}


AndroidVideoSurface::AndroidVideoSurface(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{
    setFlag(QDeclarativeItem::ItemHasNoContents(false));
    mTex = 0;
    mj_surfaceTexture=0;

    QTimer *updateTimer = new QTimer();
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(doUpdate()));
    updateTimer->setSingleShot(false);
    updateTimer->start(40);
}

/*!
 * \brief AndroidVideoSurface::paint. Overriding the native paint function to show our video.
 */
void AndroidVideoSurface::paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *)
{
    Q_UNUSED(painter);

      static bool init = false;
      if (!init) {
         init = true;
         glGenTextures(1, &mTex);
         //glBindTexture(GL_TEXTURE_2D, mTex);
         glBindTexture(GL_TEXTURE_EXTERNAL_OES, mTex);
         glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
         glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
         glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
         ATTACH_TO_CURRENT_THREAD_THEN_RET((void)0);
         jobject surfTexture = env->CallStaticObjectMethod(class_TextureHelper, mid_loadVideoTexture, mTex);/* Important piece. here the method is called that begins playback. it is passed an opengl texture id. url should be set first, then the initial methodd called internally. */
         if (!surfTexture)
            qWarning("Failed to instantiate SurfaceTexture.");
         mj_surfaceTexture = env->NewGlobalRef(surfTexture);
         DETACH_FROM_CURRENT_THREAD_THEN_RET((void)0);
      }

      if (mj_surfaceTexture) {
         ATTACH_TO_CURRENT_THREAD_THEN_RET(return);
         env->CallVoidMethod(mj_surfaceTexture, mid_updateTexture);
         DETACH_FROM_CURRENT_THREAD_THEN_RET(return);
      }

      QGLContext* context = const_cast<QGLContext*>(QGLContext::currentContext());
      context->drawTexture(QRectF(0, height(), width(), -height()), mTex, GL_TEXTURE_EXTERNAL_OES);
}

void AndroidVideoSurface::doUpdate()
{
    update(0,0, width(), height());
}
