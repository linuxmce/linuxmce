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

#ifndef ANDROIDVIDEOSURFACE_H
#define ANDROIDVIDEOSURFACE_H

#include <QDeclarativeItem>
#ifdef Q_OS_ANDROID
#include <QPainter>
#include <QGLFramebufferObject>
#include <QGLContext>
#include <jni.h>
#endif

/*!
 * \brief The AndroidVideoSurface class. This class is an interface between the JNI/Android media player and qt. What it allows us to do
 * is utilize the media player and using the framebuffer to then display the contents. It is based on a POC by
 * Author:  Luca Carlon
 * Company: -
 * Date:    02.09.2013
 * Project: TextureStreaming -https://github.com/carlonluca/TextureStreaming
 *
 * Copyright (c) 2012, 2013 Luca Carlon. All rights reserved.
 */
class AndroidVideoSurface : public QDeclarativeItem
{
    Q_OBJECT
public:
    /*!
     * \brief AndroidVideoSurface. Constructor for declarative item.
     * \param parent
     */
    explicit AndroidVideoSurface(QDeclarativeItem *parent = 0);

    /*!
     * \brief paint. Being overriden in this case.
     */
    void paint (QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
    
signals:

public slots:
    /*!
     * \brief doUpdate. Force a redraw.
     */
    void doUpdate();

private:
    QGLFramebufferObject *mp_fbo; /*!< Pointer to frame buffer object we will get from the jni */
    GLuint mTex; /*!< Opengl Textur id which will represent the frame */
    jobject mj_surfaceTexture; /*! The surface texture object read from the android media player */

};

#endif // ANDROIDVIDEOSURFACE_H
