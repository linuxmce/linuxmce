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

#This is the .pro file for the QOrbiter application using the Qt 5.4.1  toolchain for Qt on android.
#   This file is to be used when you are intending to build the application using Qt for android targeting
#   Qt version 5.*.*

CONFIG += thread c++11

# define deployment destination and target executable name
TARGET=qorbiterqt5

        DEFINES+=GLENABLED ANDROID QT5 QANDROID
        QT+= opengl script network xml core
        QT+= quick multimedia androidextras
        glmsg= yes


TRANSLATIONS += app_de.ts
# Add more folders to ship with the application, here

#NFS mount to work with skins on core directly.
#   Since Android uses remote loading, you can mount /var/www/lmce-admin/skins/android directory to /mnt/remote/android-qml on your local
#   machine. This will let you work with the qml files that are on the router as if they were local and make debugging and coding easier.
#folder_01.source = qml/android/

#folder_01.source = /mnt/remote/android-qml/
#folder_01.target = qml


#These plugins are deployed based on the Android target type. I have found this is the best method for
#Deploying custom libraries as it utilizes qmake to deploy the libraries according to how the Qt-Android interface is setup
#Tip - Use the subprojects or setup and external qml plugins to place their output into one location you main application will
#look at to find the external libraries it needs.
    x86 {
        #qmlplugins.files = ../../../platforms/Android/androidPlugins/x86/libqmlshadersplugin.so
      #  qmlplugins.files += ../../../platforms/Android/androidPlugins/x86/libandroidplugin_1_1.so
       # qmlplugins.path = /libs/x86
#
        } armeabi-v7a {
       # qmlplugins.files = ../../../platforms/Android/androidPlugins/armeabi-v7a/libqmlshadersplugin.so
      #  qmlplugins.files += ../../../platforms/Android/androidPlugins/armeabi-v7a/libandroidplugin_1_1.so
       #         qmlplugins.path = /libs/armeabi-v7a

        } else {
      # qmlplugins.files = ../../../platforms/Android/androidPlugins/armeabi/libqmlshadersplugin.so
       # qmlplugins.files += ../../../platforms/Android/androidPlugins/armeabi/libandroidplugin_1_1.so
       # qmlplugins.path = /libs/armeabi
        }

        #This is the only locally deployed qml file. Its purpose is to provide initial logic for connecting to the core,
        #and error handling, etc. It is not designer editable and should be considered as and .h / .cpp file when changing
        #and the implications of such.
        base.source = ../../qml/Index.qml
        base.target = /

        manifestFiles.source = ../../iOS/ImportedItems.qml
        manifestFiles.target = /

        #The Configuration file read on startup and used subsequently.
        #Scheduled for replacement using sqlite.
        folder_03.source = ../../config.xml
        folder_03.target = $$DESTDIR

        #Android Components plugin.
        qmlcomponents.source = ../../../platforms/Android/androidComponents
        qmlcomponents.target = imports

        #The defines that the variables listed will be deployed with the application assets. Since
        #Qt 5.1.0 on Android uses a similar deployment scheme to Necessitas, the same qmake tricks
        #apply. Mostly
        DEPLOYMENTFOLDERS = qmlcomponents base manifestFiles #folder_01
        QML_IMPORT_PATH = "androidComponents"

       # INSTALLS+= qmlplugins
        HEADERS += ../../plugins/AndroidInfo/androidsystem.h \
    ../../defineObjects/mediatypehelper.h \
    ../../RemoteCommands.h \
    ../../defineObjects/datagrids.h \
    ../../contextobjects/settinginterface.h \
    ../../datamodels/diskmediaitem.h
        SOURCES += ../../plugins/AndroidInfo/androidsystem.cpp \
    ../../defineObjects/mediatypehelper.cpp \
    ../../defineObjects/datagrids.cpp \
    ../../contextobjects/settinginterface.cpp \
    ../../datamodels/diskmediaitem.cpp

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH = imports
#QML_IMPORT_TRACE = 1



QMAKE_CXXFLAGS += -DUSE_LZO_DATAGRID

#LinuxMCE Specific include path. Linking in the app instead of against dce libs for multi-platform expediency.
INCLUDEPATH += ../../../../ ../../../../DCE/ ../../

CONFIG +=warn_off

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# The .cpp file which was generated for your project. Feel free to hack it.
#CONFIG += ordered

message(Qt version: $$[QT_VERSION])
message(Qt is installed in $$[QT_INSTALL_PREFIX])
message (Build Type: $$DEFINES)
message( Opengl Status: $$glmsg )

SOURCES += ../../main.cpp \
        ../../../qOrbiter.cpp \
        ../../../../Gen_Devices/qOrbiterBase.cpp \
        ../../qorbitermanager.cpp \
        ../../datamodels/listModel.cpp \
        ../../datamodels/gridItem.cpp \
        ../../imageProviders/gridimageprovider.cpp \
        ../../imageProviders/basicImageProvider.cpp \
        ../../datamodels/locationmodel.cpp \
        ../../datamodels/locationitem.cpp \
        ../../datamodels/usermodel.cpp \
        ../../datamodels/useritem.cpp \
        ../../datamodels/lightingscenariomodel.cpp \
        ../../datamodels/lightingscenarioitem.cpp \
        ../../datamodels/mediascenariomodel.cpp \
        ../../datamodels/mediascenarioitem.cpp \
        ../../datamodels/telecomscenariomodel.cpp \
        ../../datamodels/climatescenariomodel.cpp \
        ../../datamodels/securityscenariomodel.cpp \
        ../../datamodels/climatescenarioitem.cpp \
        ../../datamodels/securityscenarioitem.cpp \
        ../../datamodels/telecomscenarioitem.cpp \
        ../../screensaver/screensavermodule.cpp \
        ../../../../PlutoUtils/uuencode.cpp \
        ../../../../PlutoUtils/ThreadedClass.cpp \
        ../../../../PlutoUtils/Other.cpp \
        ../../../../PlutoUtils/MultiThreadIncludes.cpp \
        ../../../../PlutoUtils/minilzo.cpp \
        ../../../../PlutoUtils/md5c.cpp \
        ../../../../PlutoUtils/FileUtils.cpp \
        ../../../../PlutoUtils/CommonIncludes.cpp \
        ../../../../SerializeClass/SerializeClass.cpp \
        ../../../../DCE/Virtual_Device_Translator.cpp \
        ../../../../DCE/Socket.cpp \
        ../../../../DCE/ServerLogger.cpp \
        ../../../../DCE/PlainClientSocket.cpp \
        ../../../../DCE/MessageBuffer.cpp \
        ../../../../DCE/Message.cpp \
        ../../../../DCE/HandleRequestSocket.cpp \
        ../../../../DCE/Logger.cpp \
        ../../../../DCE/Event_Impl.cpp \
        ../../../../DCE/DCEConfig.cpp \
        ../../../../DCE/DataGrid.cpp \
        ../../../../DCE/Command_Impl.cpp \
        ../../../../DCE/AlarmManager.cpp \
        ../../../../PlutoUtils/StringUtils.cpp \
        ../../../../DCE/ClientSocket.cpp \
        ../../../../DCE/DeviceData_Base.cpp \
        ../../../../DCE/DeviceData_Impl.cpp \
        ../../datamodels/skindatamodel.cpp \
        ../../datamodels/skindataitem.cpp \
        ../../datamodels/DataModelItems/filtermodelitem.cpp \
        ../../datamodels/DataModelItems/genreitem.cpp \
        ../../../../PlutoUtils/getch.cpp \
        ../../datamodels/DataModelItems/attributesortitem.cpp \
        ../../datamodels/attributesortmodel.cpp \
        ../../datamodels/DataModelItems/mediatypeitem.cpp \
        ../../contextobjects/nowplayingclass.cpp \
        ../../contextobjects/screenparamsclass.cpp \
        ../../contextobjects/playlistclass.cpp \
        ../../contextobjects/playlistitemclass.cpp \
        ../../contextobjects/securityvideoclass.cpp \
        ../../contextobjects/epgchannellist.cpp \
        ../../playlists/epgitemclass.cpp \
        ../../datamodels/floorplanmodel.cpp \
        ../../datamodels/floorplanitem.cpp \
        ../../imageProviders/abstractimageprovider.cpp \
        ../../datamodels/DataModelItems/sleepingalarm.cpp \
        ../../contextobjects/filedetailsclass.cpp \
        ../../datamodels/avcodegrid.cpp \
        ../../contextobjects/floorplandevice.cpp \
        ../../contextobjects/screenshotattributes.cpp \
        ../../orbiterwindow.cpp \
        ../../contextobjects/screensaverclass.cpp \
        ../../contextobjects/screeninfo.cpp \
        ../../datamodels/skinloader.cpp \
        ../../datamodels/genericsetupmodel.cpp \
        ../../datamodels/genericsetupitem.cpp \
        ../../datamodels/avdevice.cpp \
        ../../contextobjects/existingorbiter.cpp \
        ../../contextobjects/modelpage.cpp \
        ../../contextobjects/avcommand.cpp \
        ../../shaders/filereader.cpp \
        ../../contextobjects/timecodemanager.cpp \
        ../../imageProviders/floorplandeviceprovider.cpp \
        # ../../Gen_Devices/qMediaPlayerBase.cpp \
        ../../contextobjects/bookmarkitem.cpp \      
        ../../contextobjects/promptdata.cpp \
        ../../datamodels/existingorbitermodel.cpp \
        ../../datamodels/devicemodel.cpp \
        ../../datamodels/attributeobject.cpp \
        ../../logger/qorbiterlogger.cpp \
        ../../datamodels/genericflatlistmodel.cpp \
        ../../datamodels/genericmodelitem.cpp \
        ../../datamodels/ActiveMediaStreams.cpp \
        ../../datamodels/listitembase.cpp \
        ../../defineObjects/linuxmcedata.cpp \
        ../../contextobjects/entertainareatimecode.cpp




#!ANDROID|!QT5{
# SOURCES+= ../../qMediaPlayer/AudioVisual/audiovisual.cpp \
#    ../../qMediaPlayer/AudioVisual/videowidgetplayer.cpp \
#../../qMediaPlayer/AudioVisual/audiowidget.cpp \
#../../qMediaPlayer/qMediaPlayer.cpp

#}


# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

#    ../../DCE/*.h \
#    ../../PlutoUtils/*.h \
#    ../../SerializeClass/*.h \
#    ../../pluto_main/*.h \
#    ../../Gen_Devices/*.h \


HEADERS += \
        ../../../../PlutoUtils/ThreadedClass.h \
        ../../../../PlutoUtils/MultiThreadIncludes.h \
        ../../../../PlutoUtils/StringUtils.h \
        ../../../../PlutoUtils/CommonIncludes.h \
        ../../../../PlutoUtils/Other.h \
        ../../../../PlutoUtils/getch.h \
        ../../../../PlutoUtils/MyStl.h \
        ../../../../DCE/DeviceData_Base.h \
        ../../../../DCE/Message.h \
        ../../../../DCE/ServerLogger.h \
        ../../../../DCE/Logger.h \
        ../../../../DCE/Virtual_Device_Translator.h \
        ../../../../DCE/PlutoLockLogger.h \
        ../../../../DCE/ClientSocket.h \
        ../../../../DCE/PlainClientSocket.h \
        ../../../../DCE/AlarmManager.h \
        ../../../../SerializeClass/SerializeClass.h \
        ../../../../PlutoUtils/FileUtils.h \
        ../../../../pluto_main/Define_DeviceCategory.h \
        ../../../../pluto_main/Define_DeviceTemplate.h \
        ../../../qOrbiter.h \
        ../../../../Gen_Devices/qOrbiterBase.h \
        ../../qorbitermanager.h \
        ../../qOrbiterData.h \
        ../../datamodels/listModel.h \
        ../../datamodels/gridItem.h \
        ../../imageProviders/gridimageprovider.h \
        ../../imageProviders/basicImageProvider.h \
        ../../datamodels/locationmodel.h \
        ../../datamodels/locationitem.h \
        ../../datamodels/usermodel.h \
        ../../datamodels/useritem.h \
        ../../datamodels/lightingscenariomodel.h \
        ../../datamodels/lightingscenarioitem.h \
        ../../datamodels/mediascenariomodel.h \
        ../../datamodels/mediascenarioitem.h \
        ../../datamodels/telecomscenariomodel.h \
        ../../datamodels/climatescenariomodel.h \
        ../../datamodels/securityscenariomodel.h \
        ../../datamodels/climatescenarioitem.h \
        ../../datamodels/securityscenarioitem.h \
        ../../datamodels/telecomscenarioitem.h \
        ../../screensaver/screensavermodule.h \
        ../../datamodels/skindatamodel.h \
        ../../datamodels/skindataitem.h \
        ../../datamodels/DataModelItems/filtermodelitem.h \
        ../../datamodels/DataModelItems/genreitem.h \
        ../../datamodels/DataModelItems/attributesortitem.h \
        ../../datamodels/attributesortmodel.h \
        ../../datamodels/DataModelItems/mediatypeitem.h \
        ../../contextobjects/filedetailsclass.h \
        ../../contextobjects/nowplayingclass.h \
        ../../contextobjects/screenparamsclass.h \
        ../../contextobjects/playlistclass.h \
        ../../contextobjects/playlistitemclass.h \
        ../../contextobjects/securityvideoclass.h \
        ../../contextobjects/epgchannellist.h \
        ../../contextobjects/screeninfo.h \
        ../../playlists/epgitemclass.h \
        ../../datamodels/floorplanmodel.h \
        ../../imageProviders/abstractimageprovider.h \
        ../../datamodels/DataModelItems/sleepingalarm.h \
        ../../datamodels/avcodegrid.h \
        ../../datamodels/floorplanimageitem.h \
        ../../contextobjects/floorplandevice.h \
        ../../contextobjects/screenshotattributes.h \
        ../../orbiterwindow.h \
        ../../contextobjects/screensaverclass.h \
        ../../datamodels/skinloader.h  \
        ../../datamodels/genericsetupmodel.h \
        ../../datamodels/genericsetupitem.h \
        ../../datamodels/avdevice.h \
        ../../contextobjects/existingorbiter.h \
        ../../contextobjects/modelpage.h \
        ../../contextobjects/avcommand.h \
        ../../shaders/filereader.h \
        ../../shaders/trace.h \
        ../../contextobjects/timecodemanager.h \
        ../../imageProviders/floorplandeviceprovider.h \
        # ../../Gen_Devices/qMediaPlayerBase.h \
        ../../contextobjects/bookmarkitem.h \     
        ../../contextobjects/promptdata.h \
        ../../datamodels/existingorbitermodel.h \
        ../../datamodels/devicemodel.h \
    ../../datamodels/attributeobject.h \
    ../../logger/qorbiterlogger.h \
    ../../datamodels/genericflatlistmodel.h \
    ../../datamodels/genericmodelitem.h \
    ../../datamodels/ActiveMediaStreams.h \
    ../../datamodels/MediaFilter.h \
    ../../datamodels/listitembase.h \
        ../../defineObjects/linuxmcedata.h \
../../defineObjects/mediatypehelper.h \
../../datamodels/DataGridHandler.h \
../../defineObjects/mediatypehelper.h \
   ../../contextobjects/entertainareatimecode.h

RESOURCES += \
        ../../skinData.qrc \
        ../../qmlFile.qrc

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/../../../platforms/Android/qt5-pkg-src
message ($$ANDROID_PACKAGE_SOURCE_DIR)

OTHER_FILES += \
    ../../../platforms/Android/qt5-pkg-src/AndroidManifest.xml \
    android/AndroidManifest.xml \
    android/res/values-rs/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/layout/splash.xml \
    android/res/values-it/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values/strings.xml \
    android/res/values/libs.xml \
    android/res/values-et/strings.xml \
    android/version.xml \
    android/res/drawable-ldpi/icon.png \
    android/res/drawable/icon.png \
    android/res/drawable/logo.png \
    android/res/drawable-mdpi/icon.png \
    android/res/drawable-hdpi/icon.png \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/src/org/kde/necessitas/origo/QtApplication.java

    ANDROID_EXTRA_LIBS += $$_PRO_FILE_PWD_/../../../platforms/Android/androidPlugins/Qt5/armeabi-v7a/libAudioVisual.so #On Android we have a special case where we need to split locations in necessitas of the lib and qmldir, unlike desktop versions.





