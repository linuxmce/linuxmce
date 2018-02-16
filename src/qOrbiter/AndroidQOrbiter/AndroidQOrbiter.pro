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

#This is the .pro file for the QOrbiter application using the Qt 5.9.0  toolchain for Qt on android.
#   This file is to be used when you are intending to build the application using Qt for android targeting
#   Qt version 5.9.0

ROOT_PATH=$$PWD/../

QT += qml quick script opengl network xml core quick multimedia androidextras quickcontrols2
CONFIG += thread c++11

TARGET=qorbiterAndroid

DEFINES+= GLENABLED ANDROID QT5 QANDROID
glmsg= yes

TRANSLATIONS += app_de.ts


QMAKE_CXXFLAGS += -DUSE_LZO_DATAGRID
INCLUDEPATH += $$ROOT_PATH/../ $$ROOT_PATH/../DCE/ $$ROOT_PATH/qOrbiter_src
RESOURCES += \
        $$ROOT_PATH/qOrbiter_src/skinData.qrc \
        $$ROOT_PATH/qOrbiter_src/qmlFile.qrc

#ANDROID_EXTRA_LIBS += $$ROOT_PATH/platforms/Android/androidPlugins/Qt5/armeabi-v7a/libAudioVisual.so
#On Android we have a special case where we need to split locations in necessitas of the lib and qmldir, unlike desktop versions.

#QML Setup
QML_ROOT_PATH = $$ROOT_PATH/qOrbiter_src/qml/
        #This is the only locally deployed qml file. Its purpose is to provide initial logic for connecting to the core,
        #and error handling, etc. It is not designer editable and should be considered as and .h / .cpp file when changing
        #and the implications of such.
        base.source =../qOrbiter_src/qml/Index.qml
        base.target = /

        manifestFiles.source =$$ROOT_PATH/qOrbiter_src/iOS/ImportedItems.qml
        manifestFiles.target = /

        #The Configuration file read on startup and used subsequently.
        #Scheduled for replacement using sqlite.
        folder_03.source = $$ROOT_PATH/qOrbiter_src/config.xml
        folder_03.target = $$DESTDIR

        DEPLOYMENTFOLDERS = base manifestFiles #folder_01


# Additional import path used to resolve QML modules in Qt Creator's code model
    QML_IMPORT_PATH = #$$ROOT_PATH/platforms/Android/androidComponents/

DEFINES += QANDROID

# Default rules for deployment.
include(deployment.pri)
include ($$ROOT_PATH/../QtCommonIncludes/PlutoUtils.pri)

message(Qt version: $$[QT_VERSION])
message(Qt is installed in $$[QT_INSTALL_PREFIX])
message (Build Type: $$DEFINES)
message( Opengl Status: $$glmsg )
message("Root Path set to" $$ROOT_PATH )


#Source

        HEADERS += \
    $$ROOT_PATH/qOrbiter_src/plugins/AndroidInfo/androidsystem.h \
    $$ROOT_PATH/qOrbiter_src/defineObjects/mediatypehelper.h \
    $$ROOT_PATH/qOrbiter_src/RemoteCommands.h \
    $$ROOT_PATH/qOrbiter_src/defineObjects/datagrids.h \
    $$ROOT_PATH/qOrbiter_src/contextobjects/settinginterface.h \
    $$ROOT_PATH/qOrbiter_src/datamodels/diskmediaitem.h \
    $$ROOT_PATH/qOrbiter_src/managerHelpers/taskshelper.h \
    $$ROOT_PATH/qOrbiter_src/managerHelpers/dcemediahelper.h \
    $$ROOT_PATH/qOrbiter_src/managerHelpers/routerhelper.h \
    $$ROOT_PATH/qOrbiter_src/managerHelpers/fontshelper.h \
       $$ROOT_PATH/qOrbiter_src/../../DCE/DeviceData_Base.h \
       $$ROOT_PATH/qOrbiter_src/../../DCE/Message.h \
       $$ROOT_PATH/qOrbiter_src/../../DCE/ServerLogger.h \
       $$ROOT_PATH/qOrbiter_src/../../DCE/Logger.h \
       $$ROOT_PATH/qOrbiter_src/../../DCE/Virtual_Device_Translator.h \
       $$ROOT_PATH/qOrbiter_src/../../DCE/PlutoLockLogger.h \
       $$ROOT_PATH/qOrbiter_src/../../DCE/ClientSocket.h \
       $$ROOT_PATH/qOrbiter_src/../../DCE/PlainClientSocket.h \
       $$ROOT_PATH/qOrbiter_src/../../DCE/AlarmManager.h \
       $$ROOT_PATH/qOrbiter_src/../../SerializeClass/SerializeClass.h \
       $$ROOT_PATH/qOrbiter_src/../../pluto_main/Define_DeviceCategory.h \
       $$ROOT_PATH/qOrbiter_src/../../pluto_main/Define_DeviceTemplate.h \
       $$ROOT_PATH/qOrbiter_src/../qOrbiter.h \
       $$ROOT_PATH/qOrbiter_src/../../Gen_Devices/qOrbiterBase.h \
       $$ROOT_PATH/qOrbiter_src/qorbitermanager.h \
       $$ROOT_PATH/qOrbiter_src/qOrbiterData.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/listModel.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/gridItem.h \
       $$ROOT_PATH/qOrbiter_src/imageProviders/gridimageprovider.h \
       $$ROOT_PATH/qOrbiter_src/imageProviders/basicImageProvider.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/locationmodel.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/locationitem.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/usermodel.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/useritem.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/lightingscenariomodel.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/lightingscenarioitem.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/mediascenariomodel.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/mediascenarioitem.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/telecomscenariomodel.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/climatescenariomodel.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/securityscenariomodel.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/climatescenarioitem.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/securityscenarioitem.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/telecomscenarioitem.h \
       $$ROOT_PATH/qOrbiter_src/screensaver/screensavermodule.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/skindatamodel.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/skindataitem.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/DataModelItems/filtermodelitem.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/DataModelItems/genreitem.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/DataModelItems/attributesortitem.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/attributesortmodel.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/DataModelItems/mediatypeitem.h \
       $$ROOT_PATH/qOrbiter_src/contextobjects/filedetailsclass.h \
       $$ROOT_PATH/qOrbiter_src/contextobjects/nowplayingclass.h \
       $$ROOT_PATH/qOrbiter_src/contextobjects/screenparamsclass.h \
       $$ROOT_PATH/qOrbiter_src/contextobjects/playlistclass.h \
       $$ROOT_PATH/qOrbiter_src/contextobjects/playlistitemclass.h \
       $$ROOT_PATH/qOrbiter_src/contextobjects/securityvideoclass.h \
       $$ROOT_PATH/qOrbiter_src/contextobjects/epgchannellist.h \
       $$ROOT_PATH/qOrbiter_src/contextobjects/screeninfo.h \
       $$ROOT_PATH/qOrbiter_src/playlists/epgitemclass.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/floorplanmodel.h \
       $$ROOT_PATH/qOrbiter_src/imageProviders/abstractimageprovider.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/DataModelItems/sleepingalarm.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/avcodegrid.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/floorplanimageitem.h \
       $$ROOT_PATH/qOrbiter_src/contextobjects/floorplandevice.h \
       $$ROOT_PATH/qOrbiter_src/contextobjects/screenshotattributes.h \
       $$ROOT_PATH/qOrbiter_src/orbiterwindow.h \
       $$ROOT_PATH/qOrbiter_src/contextobjects/screensaverclass.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/skinloader.h  \
       $$ROOT_PATH/qOrbiter_src/datamodels/genericsetupmodel.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/genericsetupitem.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/avdevice.h \
       $$ROOT_PATH/qOrbiter_src/contextobjects/existingorbiter.h \
       $$ROOT_PATH/qOrbiter_src/contextobjects/modelpage.h \
       $$ROOT_PATH/qOrbiter_src/contextobjects/avcommand.h \
       $$ROOT_PATH/qOrbiter_src/shaders/filereader.h \
       $$ROOT_PATH/qOrbiter_src/shaders/trace.h \
       $$ROOT_PATH/qOrbiter_src/contextobjects/timecodemanager.h \
       $$ROOT_PATH/qOrbiter_src/imageProviders/floorplandeviceprovider.h \
        #$$ROOT_PATH/qOrbiter_src/Gen_Devices/qMediaPlayerBase.h \
       $$ROOT_PATH/qOrbiter_src/contextobjects/bookmarkitem.h \
       $$ROOT_PATH/qOrbiter_src/contextobjects/promptdata.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/existingorbitermodel.h \
       $$ROOT_PATH/qOrbiter_src/datamodels/devicemodel.h \
   $$ROOT_PATH/qOrbiter_src/datamodels/attributeobject.h \
   $$ROOT_PATH/qOrbiter_src/logger/qorbiterlogger.h \
   $$ROOT_PATH/qOrbiter_src/datamodels/genericflatlistmodel.h \
   $$ROOT_PATH/qOrbiter_src/datamodels/genericmodelitem.h \
   $$ROOT_PATH/qOrbiter_src/datamodels/ActiveMediaStreams.h \
   $$ROOT_PATH/qOrbiter_src/datamodels/MediaFilter.h \
   $$ROOT_PATH/qOrbiter_src/datamodels/listitembase.h \
   $$ROOT_PATH/qOrbiter_src/defineObjects/linuxmcedata.h \
   $$ROOT_PATH/qOrbiter_src/defineObjects/mediatypehelper.h \
   $$ROOT_PATH/qOrbiter_src/datamodels/DataGridHandler.h \
   $$ROOT_PATH/qOrbiter_src/defineObjects/mediatypehelper.h \
   $$ROOT_PATH/qOrbiter_src/contextobjects/entertainareatimecode.h

        SOURCES +=  \
    $$ROOT_PATH/qOrbiter_src/plugins/AndroidInfo/androidsystem.cpp \
    $$ROOT_PATH/qOrbiter_src/defineObjects/mediatypehelper.cpp \
    $$ROOT_PATH/qOrbiter_src/defineObjects/datagrids.cpp \
    $$ROOT_PATH/qOrbiter_src/contextobjects/settinginterface.cpp \
    $$ROOT_PATH/qOrbiter_src/datamodels/diskmediaitem.cpp \
    $$ROOT_PATH/qOrbiter_src/managerHelpers/taskshelper.cpp \
    $$ROOT_PATH/qOrbiter_src/managerHelpers/dcemediahelper.cpp \
    $$ROOT_PATH/qOrbiter_src/managerHelpers/routerhelper.cpp \
    $$ROOT_PATH/qOrbiter_src/managerHelpers/fontshelper.cpp \
    $$ROOT_PATH/qOrbiter_src/main.cpp \
     $$ROOT_PATH/qOrbiter_src/../qOrbiter.cpp \
     $$ROOT_PATH/qOrbiter_src/../../Gen_Devices/qOrbiterBase.cpp \
    $$ROOT_PATH/qOrbiter_src/qorbitermanager.cpp \
    $$ROOT_PATH/qOrbiter_src/datamodels/listModel.cpp \
    $$ROOT_PATH/qOrbiter_src/datamodels/gridItem.cpp \
    $$ROOT_PATH/qOrbiter_src/imageProviders/gridimageprovider.cpp \
    $$ROOT_PATH/qOrbiter_src/imageProviders/basicImageProvider.cpp \
    $$ROOT_PATH/qOrbiter_src/datamodels/locationmodel.cpp \
    $$ROOT_PATH/qOrbiter_src/datamodels/locationitem.cpp \
    $$ROOT_PATH/qOrbiter_src/datamodels/usermodel.cpp \
    $$ROOT_PATH/qOrbiter_src/datamodels/useritem.cpp \
    $$ROOT_PATH/qOrbiter_src/datamodels/lightingscenariomodel.cpp \
    $$ROOT_PATH/qOrbiter_src/datamodels/lightingscenarioitem.cpp \
    $$ROOT_PATH/qOrbiter_src/datamodels/mediascenariomodel.cpp \
    $$ROOT_PATH/qOrbiter_src/datamodels/mediascenarioitem.cpp \
    $$ROOT_PATH/qOrbiter_src/datamodels/telecomscenariomodel.cpp \
    $$ROOT_PATH/qOrbiter_src/datamodels/climatescenariomodel.cpp \
    $$ROOT_PATH/qOrbiter_src/datamodels/securityscenariomodel.cpp \
    $$ROOT_PATH/qOrbiter_src/datamodels/climatescenarioitem.cpp \
    $$ROOT_PATH/qOrbiter_src/datamodels/securityscenarioitem.cpp \
    $$ROOT_PATH/qOrbiter_src/datamodels/telecomscenarioitem.cpp \
    $$ROOT_PATH/qOrbiter_src/screensaver/screensavermodule.cpp \
     $$ROOT_PATH/qOrbiter_src/../../SerializeClass/SerializeClass.cpp \
     $$ROOT_PATH/qOrbiter_src/../../DCE/Virtual_Device_Translator.cpp \
     $$ROOT_PATH/qOrbiter_src/../../DCE/Socket.cpp \
     $$ROOT_PATH/qOrbiter_src/../../DCE/ServerLogger.cpp \
     $$ROOT_PATH/qOrbiter_src/../../DCE/PlainClientSocket.cpp \
     $$ROOT_PATH/qOrbiter_src/../../DCE/MessageBuffer.cpp \
     $$ROOT_PATH/qOrbiter_src/../../DCE/Message.cpp \
     $$ROOT_PATH/qOrbiter_src/../../DCE/HandleRequestSocket.cpp \
     $$ROOT_PATH/qOrbiter_src/../../DCE/Logger.cpp \
     $$ROOT_PATH/qOrbiter_src/../../DCE/Event_Impl.cpp \
     $$ROOT_PATH/qOrbiter_src/../../DCE/DCEConfig.cpp \
     $$ROOT_PATH/qOrbiter_src/../../DCE/DataGrid.cpp \
     $$ROOT_PATH/qOrbiter_src/../../DCE/Command_Impl.cpp \
     $$ROOT_PATH/qOrbiter_src/../../DCE/AlarmManager.cpp \
     $$ROOT_PATH/qOrbiter_src/../../DCE/ClientSocket.cpp \
     $$ROOT_PATH/qOrbiter_src/../../DCE/DeviceData_Base.cpp \
     $$ROOT_PATH/qOrbiter_src/../../DCE/DeviceData_Impl.cpp \
     $$ROOT_PATH/qOrbiter_src/datamodels/skindatamodel.cpp \
     $$ROOT_PATH/qOrbiter_src/datamodels/skindataitem.cpp \
     $$ROOT_PATH/qOrbiter_src/datamodels/DataModelItems/filtermodelitem.cpp \
     $$ROOT_PATH/qOrbiter_src/datamodels/DataModelItems/genreitem.cpp \
      $$ROOT_PATH/qOrbiter_src/datamodels/DataModelItems/attributesortitem.cpp \
      $$ROOT_PATH/qOrbiter_src/datamodels/attributesortmodel.cpp \
     $$ROOT_PATH/qOrbiter_src/datamodels/DataModelItems/mediatypeitem.cpp \
      $$ROOT_PATH/qOrbiter_src/contextobjects/nowplayingclass.cpp \
     $$ROOT_PATH/qOrbiter_src/contextobjects/screenparamsclass.cpp \
      $$ROOT_PATH/qOrbiter_src/contextobjects/playlistclass.cpp \
       $$ROOT_PATH/qOrbiter_src/contextobjects/playlistitemclass.cpp \
     $$ROOT_PATH/qOrbiter_src/contextobjects/securityvideoclass.cpp \
     $$ROOT_PATH/qOrbiter_src/contextobjects/epgchannellist.cpp \
     $$ROOT_PATH/qOrbiter_src/playlists/epgitemclass.cpp \
     $$ROOT_PATH/qOrbiter_src/datamodels/floorplanmodel.cpp \
     $$ROOT_PATH/qOrbiter_src/datamodels/floorplanitem.cpp \
     $$ROOT_PATH/qOrbiter_src/imageProviders/abstractimageprovider.cpp \
     $$ROOT_PATH/qOrbiter_src/datamodels/DataModelItems/sleepingalarm.cpp \
     $$ROOT_PATH/qOrbiter_src/contextobjects/filedetailsclass.cpp \
     $$ROOT_PATH/qOrbiter_src/datamodels/avcodegrid.cpp \
     $$ROOT_PATH/qOrbiter_src/contextobjects/floorplandevice.cpp \
     $$ROOT_PATH/qOrbiter_src/contextobjects/screenshotattributes.cpp \
     $$ROOT_PATH/qOrbiter_src/orbiterwindow.cpp \
     $$ROOT_PATH/qOrbiter_src/contextobjects/screensaverclass.cpp \
     $$ROOT_PATH/qOrbiter_src/contextobjects/screeninfo.cpp \
     $$ROOT_PATH/qOrbiter_src/datamodels/skinloader.cpp \
     $$ROOT_PATH/qOrbiter_src/datamodels/genericsetupmodel.cpp \
     $$ROOT_PATH/qOrbiter_src/datamodels/genericsetupitem.cpp \
     $$ROOT_PATH/qOrbiter_src/datamodels/avdevice.cpp \
     $$ROOT_PATH/qOrbiter_src/contextobjects/existingorbiter.cpp \
     $$ROOT_PATH/qOrbiter_src/contextobjects/modelpage.cpp \
     $$ROOT_PATH/qOrbiter_src/contextobjects/avcommand.cpp \
     $$ROOT_PATH/qOrbiter_src/shaders/filereader.cpp \
     $$ROOT_PATH/qOrbiter_src/contextobjects/timecodemanager.cpp \
     $$ROOT_PATH/qOrbiter_src/imageProviders/floorplandeviceprovider.cpp \
      #$$ROOT_PATH/qOrbiter_src/Gen_Devices/qMediaPlayerBase.cpp \
     $$ROOT_PATH/qOrbiter_src/contextobjects/bookmarkitem.cpp \
     $$ROOT_PATH/qOrbiter_src/contextobjects/promptdata.cpp \
     $$ROOT_PATH/qOrbiter_src/datamodels/existingorbitermodel.cpp \
     $$ROOT_PATH/qOrbiter_src/datamodels/devicemodel.cpp \
     $$ROOT_PATH/qOrbiter_src/datamodels/attributeobject.cpp \
     $$ROOT_PATH/qOrbiter_src/logger/qorbiterlogger.cpp \
     $$ROOT_PATH/qOrbiter_src/datamodels/genericflatlistmodel.cpp \
     $$ROOT_PATH/qOrbiter_src/datamodels/genericmodelitem.cpp \
     $$ROOT_PATH/qOrbiter_src/datamodels/ActiveMediaStreams.cpp \
     $$ROOT_PATH/qOrbiter_src/datamodels/listitembase.cpp \
     $$ROOT_PATH/qOrbiter_src/defineObjects/linuxmcedata.cpp \
     $$ROOT_PATH/qOrbiter_src/contextobjects/entertainareatimecode.cpp

DISTFILES += \
    android/AndroidManifest.xml \
    android/res/values/libs.xml \
    android/build.gradle

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS =
}
