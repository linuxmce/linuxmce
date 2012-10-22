#
# This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org
#    Langston Ball  golgoj4@gmail.com

#    QOrbiter is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.

#    QOrbiter is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.

#    You should have received a copy of the GNU General Public License
#    along with QOrbiter.  If not, see <http://www.gnu.org/licenses/>.

# NOTE!! You should build the AudioVisual Plugin in the Plugins directory and make sure its output path is set to the 'imports folder!'

CONFIG += thread
# define deployment destination and target executable name

ANDROID{
DESTDIR = ../QOrbiter-Android-Arm-$$QT_VERSION
} else {
!linux-rasp-pi-g++{
DESTDIR = ../QOrbiter-build-$$QT_VERSION
}
}

contains(QT_VERSION,4.8.*){
message("$$QT_VERSION Core")
DEFINES+=QT4_8

}

contains(QT_VERSION,4.7.*){
message("$$QT_VERSION Core")
DEFINES+=QT4_7
}


contains(QT_VERSION,5.0.*){
message("$$QT_VERSION Core")
DEFINES+=QT5
}


opengl{
TARGET = qorbiter-$$QT_VERSION-core-gl
DEFINES+=GLENABLED
glmsg= yes
QT+= opengl

}else{

TARGET =qorbiter-$$QT_VERSION
glmsg = no
}

ANDROID{
TARGET=qorbiter-$$QT_VERSION
}

symbian:TARGET.UID3 = 0xE15A481D

# Smart Installer package's UID
# This UID is from the protected range and therefore the package will
# fail to install if self-signed. By default qmake uses the unprotected
# range value if unprotected UID is defined for the application and
# 0x2002CCCF value if protected UID is given to the application
#symbian:DEPLOYMENT.installer_header = 0x2002CCCF

# Allow network access on Symbian
symbian:TARGET.CAPABILITY += NetworkServices

# When we do stuff for Symbian, why not define the target in here...
symbian {
    DEFINES += for_symbian
    INCLUDEPATH += $$EPOCROOT/epoc32/include/stdapis/stlport
    INCLUDEPATH += $$EPOCROOT/epoc32/include/stdapis/stlport/stl
    INCLUDEPATH += $$EPOCROOT/epoc32/include/libc
    DEFINES += Q_OS_SYMBIAN
}
TRANSLATIONS += app_de.ts
# Add more folders to ship with the application, here
#linux deploy configuration
linux-g++{
folder_01.source = qml/desktop
folder_01.target = $$DESTDIR/qml

folder_05.source = qml/template
folder_05.target = $$DESTDIR/template

folder_03.source = config.xml
folder_03.target = $$DESTDIR
DEFINES += for_desktop


plugins_folder.source = imports/
plugins_folder.target = $$DESTDIR
DEPLOYMENTFOLDERS+= plugins_folder
DEPLOYMENTFOLDERS += folder_01  folder_03 folder_05
}

#windows deployment and module config
WIN32{
folder_01.source = qml/desktop
folder_01.target = $$DESTDIR/qml

folder_05.source = plugins
folder_05.target = $$DESTDIR/plugins

files_01.source = "../qOrbiter_src/config.xml/"
files_01.path =$$DESTDIR/

DEFINES += for_windows
plugins_folder.source = imports/
plugins_folder.target = $$DESTDIR
DEPLOYMENTFOLDERS+= plugins_folder
DEPLOYMENTFOLDERS += folder_01 files_01 folder_05
}

#android simulation config
for_android{
folder_01.source = qml/android/
folder_01.target = $$DESTDIR/qml

folder_05.source = qml/template
folder_05.target = $$DESTDIR/qml

folder_03.source = config.xml
folder_03.target = $$DESTDIR

    qmlcomponents.source = androidComponents/Qt \ androidComponents/com
    qmlcomponents.target = qml/imports/

qmlbase.source = qml/android/base.qml
qmlbase.target = $$DESTDIR/qml

    qmlplugins.files = \
 androidPlugins/libqmlshadersplugin.so \
androidPlugins/libandroidplugin_1_1.so \

        x86 {
            qmlplugins.path = /libs/x86
        } else: armeabi-v7a {
            qmlplugins.path = /libs/armeabi-v7a
        } else {
            qmlplugins.path = /libs/armeabi
        }

DEPLOYMENTFOLDERS+= qmlcomponents
    INSTALLS+= qmlplugins

DEFINES+=for_android

DEPLOYMENTFOLDERS += folder_01 folder_02 folder_03 folder_05
}

#freemantle config
for_freemantle{
folder_01.source = qml/freemantle
folder_01.target = $$DESTDIR/qml

folder_05.source = qml/template
folder_05.target = $$DESTDIR/qml

folder_02.source= img
folder_02.target=     #left blank so it will appear in the root

folder_03.source = config.xml
folder_03.target = $$DESTDIR/config
DEFINES += for_freemantle
DEPLOYMENTFOLDERS += folder_01 folder_02 folder_03 folder_05
}

#harmattan config
for_harmattan{
folder_01.source = qml/harmattan
folder_01.target =
folder_05.source = qml/template
folder_05.target = $$DESTDIR/qml
folder_02.source= img
folder_02.target=     #left blank so it will appear in the root
DEFINES += for_harmattan
DEPLOYMENTFOLDERS += folder_01 folder_02 folder_05
}

#mac desktop config
macx{
    APP_RESOURCES_PATH=../../../$$DESTDIR/$$TARGET".app"/Contents/resources
    folder_01.source = qml/desktop
    folder_01.target = $$APP_RESOURCES_PATH/qml
    folder_02.source= img
    folder_02.target= $$APP_RESOURCES_PATH   #left blank so it will appear in the root
    folder_05.source = qml/template
    folder_05.target = $$DESTDIR/qml
    folder_03.source = config.xml
    folder_03.target = $$APP_RESOURCES_PATH
    ICON = osxicons.icns
DEPLOYMENTFOLDERS += folder_01 folder_02 folder_03 folder_05
}

#android deployment
ANDROID{
#nfs mount to work with skins on core directly
#folder_01.source = ../../../../mnt/remote/android-qml/android
#folder_01.source = qml/android/
#folder_01.target = qml

base.source = qml/android/Base.qml
base.target = qml/

folder_03.source = config.xml
folder_03.target = $$DESTDIR

qmlcomponents.source = androidComponents
qmlcomponents.target = imports/

qmlplugins.files = \
androidPlugins/libqmlshadersplugin.so \
androidPlugins/libandroidplugin_1_1.so

        x86 {
            qmlplugins.path = /libs/x86
        } else: armeabi-v7a {
            qmlplugins.path = /libs/armeabi-v7a
        } else {
            qmlplugins.path = /libs/armeabi
        }

    INSTALLS+= qmlplugins
    DEFINES+=ANDROID GLENABLED
DEPLOYMENTFOLDERS = qmlcomponents base #folder_01
QML_IMPORT_PATH = "androidComponents"
}

linux-rasp-pi-g++{

folder_01.source= qml/rpi
folder_01.target= qml

folder_02.source = config.xml
folder_02.target = config

DEFINES+=RPI
DEPLOYMENTFOLDERS += folder_01 folder_02
QT+= qml
}

# Additional import path used to resolve QML modules in Creator's code model

#QML_IMPORT_TRACE = 1

symbian:TARGET.UID3 = 0xE0D07D4D
QMAKE_CXXFLAGS += -DUSE_LZO_DATAGRID

INCLUDEPATH += ../../ ../../DCE/

QT4{
QT+= webkit declarative phonon
}

QT5{
QT+= qml quick webkit mobility widgets qtcore
}

macx{
    QT += xml
    QT += network
}

!macx{
    LIBS += -lQtXml
}

win32{
INCLUDEPATH += ../qorbiter-pthreads/Prebuilt/include/
LIBS+= -L../qorbiter-pthreads/Prebuilt/lib/ -lpthreadVC2
LIBS +=  -lWS2_32 -luser32
#LIBS +=  -lpthreadVC2
DEFINES += USE_MYSQL_WRAPPER
QT += network
}

CONFIG +=warn_off

# Smart Installer package's UID
# This UID is from the protected range and therefore the package will
# fail to install if self-signed. By default qmake uses the unprotected
# range value if unprotected UID is defined for the application and
# 0x2002CCCF value if protected UID is given to the application
#symbian:DEPLOYMENT.installer_header = 0x2002CCCF

# Allow network access on Symbian
symbian:TARGET.CAPABILITY += NetworkServices

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=
message(Qt version: $$[QT_VERSION])
message(Qt is installed in $$[QT_INSTALL_PREFIX])
message (Build Type: $$DEFINES)
message( Opengl Status: $$glmsg )


# The .cpp file which was generated for your project. Feel free to hack it.
#CONFIG += ordered
#SOURCES += plugins/googleWeatherPlugin/googleweatherplugin.pro

SOURCES += main.cpp \
        ../../Gen_Devices/qOrbiterBase.cpp \
    ../qOrbiter.cpp \   
    qorbitermanager.cpp \    
    datamodels/listModel.cpp \
    datamodels/gridItem.cpp \
    imageProviders/gridimageprovider.cpp \
    imageProviders/basicImageProvider.cpp \
    datamodels/orbiterbuttonitem.cpp \
    datamodels/locationmodel.cpp \
    datamodels/locationitem.cpp \
    datamodels/usermodel.cpp \
    datamodels/useritem.cpp \
    datamodels/lightingscenariomodel.cpp \
    datamodels/lightingscenarioitem.cpp \
    datamodels/mediascenariomodel.cpp \
    datamodels/mediascenarioitem.cpp \
    datamodels/telecomscenariomodel.cpp \
    datamodels/climatescenariomodel.cpp \
    datamodels/securityscenariomodel.cpp \
    datamodels/climatescenarioitem.cpp \
    datamodels/securityscenarioitem.cpp \
    datamodels/telecomscenarioitem.cpp \
    screensaver/screensavermodule.cpp \
    ../../PlutoUtils/uuencode.cpp \
    ../../PlutoUtils/ThreadedClass.cpp \
    ../../PlutoUtils/Other.cpp \
    ../../PlutoUtils/MultiThreadIncludes.cpp \
    ../../PlutoUtils/minilzo.cpp \
    ../../PlutoUtils/md5c.cpp \
    ../../PlutoUtils/FileUtils.cpp \
    ../../PlutoUtils/CommonIncludes.cpp \
    ../../SerializeClass/SerializeClass.cpp \
    ../../DCE/Virtual_Device_Translator.cpp \
    ../../DCE/Socket.cpp \
    ../../DCE/ServerLogger.cpp \
    ../../DCE/PlainClientSocket.cpp \
    ../../DCE/MessageBuffer.cpp \
    ../../DCE/Message.cpp \
    ../../DCE/HandleRequestSocket.cpp \
    ../../DCE/Logger.cpp \
    ../../DCE/Event_Impl.cpp \
    ../../DCE/DCEConfig.cpp \
    ../../DCE/DataGrid.cpp \
    ../../DCE/Command_Impl.cpp \
    ../../DCE/AlarmManager.cpp \
    ../../PlutoUtils/StringUtils.cpp \
    ../../DCE/ClientSocket.cpp \
    ../../DCE/DeviceData_Base.cpp \
    ../../DCE/DeviceData_Impl.cpp \
    datamodels/skindatamodel.cpp \
    datamodels/skindataitem.cpp \
    datamodels/filtermodel.cpp \
    datamodels/genremodel.cpp \
    datamodels/attributemodel.cpp \
    datamodels/DataModelItems/filtermodelitem.cpp \
    datamodels/DataModelItems/genreitem.cpp \
    ../../PlutoUtils/getch.cpp \
    datamodels/DataModelItems/attributesortitem.cpp \
    datamodels/attributesortmodel.cpp \
    datamodels/mediatypemodel.cpp \
    datamodels/DataModelItems/mediatypeitem.cpp \
    datamodels/filedetailsmodel.cpp \
    datamodels/DataModelItems/filedetailsitem.cpp \
    contextobjects/nowplayingclass.cpp \
    datamodels/DatagridClasses/datagriditemmodelclass.cpp \
    contextobjects/screenparamsclass.cpp \
    contextobjects/playlistclass.cpp \
    contextobjects/playlistitemclass.cpp \
    datamodels/DatagridClasses/datagriditem.cpp \
    contextobjects/securityvideoclass.cpp \
    contextobjects/epgchannellist.cpp \
    playlists/epgitemclass.cpp \
    datamodels/floorplanmodel.cpp \
    datamodels/floorplanitem.cpp \
    imageProviders/abstractimageprovider.cpp \
    datamodels/DataModelItems/sleepingalarm.cpp \
    contextobjects/filedetailsclass.cpp \
    avcodegrid.cpp \
    avitem.cpp \
    contextobjects/floorplandevice.cpp \
    contextobjects/screenshotattributes.cpp \
    threadedClasses/threadedsplash.cpp \
    orbiterwindow.cpp \
    contextobjects/screensaverclass.cpp \
    datamodels/skinloader.cpp \
    datamodels/genericsetupmodel.cpp \
    datamodels/genericsetupitem.cpp \
    dgmanager.cpp \
    datamodels/avdevice.cpp \
    contextobjects/existingorbiter.cpp \
    contextobjects/modelpage.cpp \
    contextobjects/avcommand.cpp \
    shaders/filereader.cpp \
    contextobjects/timecodemanager.cpp \
    imageProviders/floorplandeviceprovider.cpp \   
   # ../../Gen_Devices/qMediaPlayerBase.cpp \
    contextobjects/bookmarkitem.cpp \
    plugins/GoogleWeather/googleweather.cpp \
    contextobjects/promptdata.cpp \



#!ANDROID|!QT5{
# SOURCES+= ../../qMediaPlayer/AudioVisual/audiovisual.cpp \
#    ../../qMediaPlayer/AudioVisual/videowidgetplayer.cpp \
#../../qMediaPlayer/AudioVisual/audiowidget.cpp \
#../../qMediaPlayer/qMediaPlayer.cpp

#}


# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

#    ../../DCE/*.h \
#    ../../PlutoUtils/*.h \
#    ../../SerializeClass/*.h \
#    ../../pluto_main/*.h \
#    ../../Gen_Devices/*.h \


HEADERS += \
    ../../PlutoUtils/ThreadedClass.h \
    ../../PlutoUtils/MultiThreadIncludes.h \
    ../../PlutoUtils/StringUtils.h \
    ../../PlutoUtils/CommonIncludes.h \
    ../../PlutoUtils/Other.h \
    ../../PlutoUtils/getch.h \
    ../../PlutoUtils/MyStl.h \
    ../../DCE/DeviceData_Base.h \
    ../../DCE/Message.h \
    ../../DCE/ServerLogger.h \
    ../../DCE/Logger.h \
    ../../DCE/Virtual_Device_Translator.h \
    ../../DCE/PlutoLockLogger.h \
    ../../DCE/ClientSocket.h \
    ../../DCE/PlainClientSocket.h \
    ../../DCE/AlarmManager.h \
    ../../SerializeClass/SerializeClass.h \
    ../../PlutoUtils/FileUtils.h \
    ../../pluto_main/Define_DeviceCategory.h \
    ../../pluto_main/Define_DeviceTemplate.h \
    ../qOrbiter.h \
    ../../Gen_Devices/qOrbiterBase.h \   
    qorbitermanager.h \
    qOrbiterData.h \
    datamodels/listModel.h \
    datamodels/gridItem.h \
    imageProviders/gridimageprovider.h \
    imageProviders/basicImageProvider.h \
    datamodels/orbiterbuttonitem.h \
    datamodels/locationmodel.h \
    datamodels/locationitem.h \
    datamodels/usermodel.h \
    datamodels/useritem.h \
    datamodels/lightingscenariomodel.h \
    datamodels/lightingscenarioitem.h \
    datamodels/mediascenariomodel.h \
    datamodels/mediascenarioitem.h \
    datamodels/telecomscenariomodel.h \
    datamodels/climatescenariomodel.h \
    datamodels/securityscenariomodel.h \
    datamodels/climatescenarioitem.h \
    datamodels/securityscenarioitem.h \
    datamodels/telecomscenarioitem.h \
    screensaver/screensavermodule.h \
    datamodels/skindatamodel.h \
    datamodels/skindataitem.h \
    datamodels/filtermodel.h \
    datamodels/genremodel.h \
    datamodels/attributemodel.h \
    datamodels/DataModelItems/filtermodelitem.h \
    datamodels/DataModelItems/genreitem.h \
    datamodels/DataModelItems/attributesortitem.h \
    datamodels/attributesortmodel.h \
    datamodels/mediatypemodel.h \
    datamodels/DataModelItems/mediatypeitem.h \
    datamodels/filedetailsmodel.h \
    datamodels/DataModelItems/filedetailsitem.h \
    contextobjects/filedetailsclass.h \
    contextobjects/nowplayingclass.h \
    datamodels/DatagridClasses/datagriditemmodelclass.h \
    contextobjects/screenparamsclass.h \
    contextobjects/playlistclass.h \
    contextobjects/playlistitemclass.h \
    datamodels/DatagridClasses/datagriditem.h \
    contextobjects/securityvideoclass.h \
    contextobjects/epgchannellist.h \
    playlists/epgitemclass.h \
    datamodels/floorplanmodel.h \
    imageProviders/abstractimageprovider.h \
    datamodels/DataModelItems/sleepingalarm.h \
    avcodegrid.h \
    avitem.h \
    datamodels/floorplanimageitem.h \
    contextobjects/floorplandevice.h \
    contextobjects/screenshotattributes.h \
    threadedClasses/threadedsplash.h \
    orbiterwindow.h \
    contextobjects/screensaverclass.h \
    datamodels/skinloader.h  \
    datamodels/genericsetupmodel.h \
    datamodels/genericsetupitem.h \
    dgmanager.h \
    datamodels/avdevice.h \
    contextobjects/existingorbiter.h \
    contextobjects/modelpage.h \
    contextobjects/avcommand.h \
    shaders/filereader.h \
    shaders/trace.h \
    contextobjects/timecodemanager.h \
    imageProviders/floorplandeviceprovider.h \    
   # ../../Gen_Devices/qMediaPlayerBase.h \
    contextobjects/bookmarkitem.h \
    plugins/GoogleWeather/googleweather.h \
    contextobjects/promptdata.h \



#!ANDROID||!QT5{
# HEADERS+=   ../../qMediaPlayer/AudioVisual/audiovisual.h \
#    ../../qMediaPlayer/AudioVisual/videowidgetplayer.h \
#../../qMediaPlayer/AudioVisual/audiowidget.h\
#../../qMediaPlayer/qMediaPlayer.h
#}

OTHER_FILES += Readme.txt \
    qml/Splash.qml \
    qml/SetupNewOrbiter.qml \
    OrbiterVariables.txt \
    qml/harmattan/js/nowPlayingWorker.js \
    config.xml \    
    qml/effects/EffectVignette.qml \
    qml/shaders/vignette.fsh \
    qml/shaders/gaussianblur_v.fsh \
    qml/shaders/gaussianblur_h.fsh \
    qml/effects/EffectGaussianBlur.qml \
    qmldir \
    qml/SplashView.qml \
    ../../../../mnt/remote/android-qml/android/tablet/default/effects/qmldir


for_harmattan{
OTHER_FILES= \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \
}


RESOURCES += \
    skinData.qrc

contains(MEEGO_EDITION,harmattan) {
    desktopfile.files = $${TARGET}.desktop
    desktopfile.path = /usr/share/applications
    INSTALLS += desktopfile
}

