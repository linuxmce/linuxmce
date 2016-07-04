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

# NOTE2!! Please add any source or header file to the relevant part of this file.
#         This ensures that stuff like remote building will work.
#         Thanks
#		possy


CONFIG += thread c++11
# define deployment destination and target executable name
include (../../QtCommonIncludes/PlutoUtils.pri)
local-development{
    android-g++{ DESTDIR = ../QOrbiter-Android-Arm-$$QT_VERSION }
     else  {
        !linux-rasp-pi-g++{ DESTDIR = ../QOrbiter-build-$$QT_VERSION }
            }
} else {
    DESTDIR = ../build-output
}

contains(QT_VERSION,4.8.*){
        message("$$QT_VERSION Core")
        DEFINES+=QT4_8

        QT+=opengl
!android-g++{
        QT+= webkit declarative phonon
}
    include(qmlapplicationviewer/qmlapplicationviewer.pri)
}

simulate{
DEFINES+=simulate
}

contains(QT_VERSION,4.7.*){
        message("$$QT_VERSION Core")
        DEFINES+=QT4_7
        QT+= webkit declarative phonon
    include(qmlapplicationviewer/qmlapplicationviewer.pri)

}

contains(QT_VERSION,5.*.*){
        message("$$QT_VERSION Core")
        DEFINES+=QT5
        CONFIG+= opengl
        QT+= script widgets
      include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
     #   include(../platforms/Android/QmlVlc/QmlVlc/QmlVlc.pri)
}

android-g++{
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

TRANSLATIONS = \
        translations/en_US.ts \
        translations/de_DE.ts \
        translations/fr_FR.ts \
        translations/es_ES.ts \

# Add more folders to ship with the application, here
#linux deploy configuration

common-folder.source = qml/skins-common
common-folder.target = $$DESTDIR/qml

plugins_folder.source = imports
plugins_folder.target = $$DESTDIR


#DEPLOYMENTFOLDERS+=common-folder

linux-rasp-pi-g++ || pi{

        DEFINES+=RPI GLENABLED
        DEFINES-=for_desktop
       # DEPLOYMENTFOLDERS += folder_02  #folder_01
        QT+= qml
        QT-=declarative
        DESTDIR=""
        target.path=/usr/pluto/bin
        INSTALLS+=target
                runner.source=../buildScripts/rpi/runSingleQorbiter.sh
                runner.path=/usr/pluto/bin
                DEPLOYMENTFOLDERS+=runner
        plugins_folder.source = imports
        plugins_folder.target = $$DESTDIR

       # DEPLOYMENTFOLDERS -= plugins_folder
}

linux-g++{
      #  TARGET = qorbiter-$$QT_VERSION-core-gl

        contains(QT_VERSION,4.*.*){
                CONFIG+=opengl
                DEFINES+= GLENABLED for_desktop
                QT += script opengl
                folder_01.source = qml
                folder_01.target = $$DESTDIR/qml
                target.path=/usr/pluto/bin
        }

        contains(QT_VERSION,5.*.*){
                QT+= qml quick script widgets
                QT-= declarative
             #   folder_01.source = qml
               # folder_01.target = $$DESTDIR/qml/
                DEFINES += for_desktop GLENABLED
                glmsg=scenegraph

        }
!RPI{
      # DEPLOYMENTFOLDERS+= plugins_folder

       # folder_03.source = config.xml
       # folder_03.target = $$DESTDIR
      #  DEPLOYMENTFOLDERS+=folder_01
        QML_IMPORT_PATH=imports
}

}

local-development{
        opengl{
                TARGET = qorbiter-$$QT_VERSION-core-gl
                 DEFINES+= GLENABLED
                 glmsg= yes
                 QT+= opengl
        }else{
                TARGET =qorbiter
                glmsg = no
               }
} else {
        opengl{
                TARGET = qorbiter-core-gl
                 DEFINES+= GLENABLED
                 glmsg= yes
                 QT+= opengl
        }else{
                TARGET =qorbiter
                glmsg = no
               }
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
        #DEPLOYMENTFOLDERS += folder_01
}



#mac desktop config
macx-clang{
        APP_RESOURCES_PATH=../../../$$DESTDIR/$$TARGET".app"/Contents/resources
        ICON = ../platforms/osx/osxicons.icns
        plugins_folder.source = imports
        plugins_folder.target = ../../../$$DESTDIR/$$TARGET".app"/Contents/MacOS/

      message("You Must copy the libraries at $$[QT_INSTALL_PREFIX]/qml to $$QTDIR/imports before deploying" )
       # DEPLOYMENTFOLDERS +=  folder_03 plugins_folder
        DEFINES+=MACBUILD
        QML_IMPORT_PATH=../../../$$DESTDIR/$$TARGET".app"/Contents/MacOS/imports
       # QT-=bundle
}

index.source=qml/Index.qml
index.target=$$DESTDIR

folder_02.source=qml/skins
folder_02.target=$$DESTDIR

folder_03.source=qml/splash
folder_03.target=$$DESTDIR

folder_04.source=qml/images
folder_04.target=$$DESTDIR

#folder_01.target=qml
DEPLOYMENTFOLDERS +=index folder_02 folder_04 folder_03 plugins_folder

linux-rasp-pi-g++{

        DEPLOYMENTFOLDERS -= plugins_folder
}

macx-clang{
        index.target=$$APP_RESOURCES_PATH

        folder_02.source=qml/skins
        folder_02.target=$$APP_RESOURCES_PATH

        folder_03.source=qml/splash
        folder_03.target=$$APP_RESOURCES_PATH

        folder_04.source=qml/images
        folder_04.target=$$APP_RESOURCES_PATH
}
# Additional import path used to resolve QML modules in Creator's code model

#QML_IMPORT_TRACE = 1

symbian:TARGET.UID3 = 0xE0D07D4D
QMAKE_CXXFLAGS += -DUSE_LZO_DATAGRID
#LinuxMCE Specific include path. Linking in the app instead of against dce libs for multi-platform expediency.
INCLUDEPATH += ../../ ../../DCE/




noqrc{
DEFINES+=NOQRC
RESOURCES-=qmlFile.qrc
RESOURCES-=skinData.qrc
} else {
RESOURCES+=qmlFile.qrc
RESOURCES+= skinData.qrc
}



!macx-clang{
        contains(QT_VERSION,4.*.*){
        LIBS += -lQtXml
        LIBS += -lQtNetwork

        }
        else:contains(QT_VERSION,5.*.*){
        QT += xml
        QT += network
        }

} else {
        QT += xml
        QT += network
}

#windows builds require a special pthreads.
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

# The .cpp file which was generated for your project. Feel free to hack it.
#CONFIG += ordered

message(Qt version: $$[QT_VERSION])
message(Qt is installed in $$[QT_INSTALL_PREFIX])
message (Build Type: $$DEFINES)
message( Opengl Status: $$glmsg )
message( Output Path $$DESTDIR )
message(Deploying folders $$DEPLOYMENTFOLDERS)

SOURCES += \
        main.cpp \
         ../qOrbiter.cpp \
        ../../Gen_Devices/qOrbiterBase.cpp \
        qorbitermanager.cpp \
        datamodels/listModel.cpp \
        datamodels/gridItem.cpp \
        imageProviders/gridimageprovider.cpp \
        imageProviders/basicImageProvider.cpp \
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
        ../../DCE/ClientSocket.cpp \
        ../../DCE/DeviceData_Base.cpp \
        ../../DCE/DeviceData_Impl.cpp \
        datamodels/skindatamodel.cpp \
        datamodels/skindataitem.cpp \
        datamodels/DataModelItems/filtermodelitem.cpp \
        datamodels/DataModelItems/genreitem.cpp \        
        datamodels/DataModelItems/attributesortitem.cpp \
        datamodels/attributesortmodel.cpp \
        datamodels/DataModelItems/mediatypeitem.cpp \
        contextobjects/nowplayingclass.cpp \
        contextobjects/screenparamsclass.cpp \
        contextobjects/playlistclass.cpp \
        contextobjects/playlistitemclass.cpp \
        contextobjects/securityvideoclass.cpp \
        contextobjects/epgchannellist.cpp \
        playlists/epgitemclass.cpp \
        datamodels/floorplanmodel.cpp \
        datamodels/floorplanitem.cpp \
        imageProviders/abstractimageprovider.cpp \
        datamodels/DataModelItems/sleepingalarm.cpp \
        contextobjects/filedetailsclass.cpp \
        datamodels/avcodegrid.cpp \
        contextobjects/floorplandevice.cpp \
        contextobjects/screenshotattributes.cpp \
        orbiterwindow.cpp \
        contextobjects/screensaverclass.cpp \
        datamodels/skinloader.cpp \
        datamodels/genericsetupmodel.cpp \
        datamodels/genericsetupitem.cpp \
        datamodels/avdevice.cpp \
        contextobjects/existingorbiter.cpp \
        contextobjects/modelpage.cpp \
        contextobjects/avcommand.cpp \
        shaders/filereader.cpp \
        contextobjects/timecodemanager.cpp \
        imageProviders/floorplandeviceprovider.cpp \
        # ../../Gen_Devices/qMediaPlayerBase.cpp \
        contextobjects/bookmarkitem.cpp \
        contextobjects/promptdata.cpp \
        datamodels/existingorbitermodel.cpp \
        datamodels/devicemodel.cpp \
        datamodels/attributeobject.cpp \
        logger/qorbiterlogger.cpp \
        datamodels/genericflatlistmodel.cpp \
        datamodels/genericmodelitem.cpp \
        datamodels/ActiveMediaStreams.cpp \
        datamodels/listitembase.cpp \
        defineObjects/linuxmcedata.cpp \
        defineObjects/mediatypehelper.cpp \
        defineObjects/datagrids.cpp \
    contextobjects/screeninfo.cpp \
    contextobjects/settinginterface.cpp \
    contextobjects/entertainareatimecode.cpp \
    datamodels/diskmediaitem.cpp \
    managerHelpers/taskshelper.cpp \
    managerHelpers/dcemediahelper.cpp \
    managerHelpers/routerhelper.cpp



# Please do not modify the following two lines. Required for deployment.
 qtcAddDeployment()



#    ../../DCE/*.h \
#    ../../PlutoUtils/*.h \
#    ../../SerializeClass/*.h \
#    ../../pluto_main/*.h \
#    ../../Gen_Devices/*.h \


HEADERS += \
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
        datamodels/DataModelItems/filtermodelitem.h \
        datamodels/DataModelItems/genreitem.h \
        datamodels/DataModelItems/attributesortitem.h \
        datamodels/attributesortmodel.h \
        datamodels/DataModelItems/mediatypeitem.h \
        contextobjects/filedetailsclass.h \
        contextobjects/nowplayingclass.h \
        contextobjects/screenparamsclass.h \
        contextobjects/playlistclass.h \
        contextobjects/playlistitemclass.h \
        contextobjects/securityvideoclass.h \
        contextobjects/epgchannellist.h \
        playlists/epgitemclass.h \
        datamodels/floorplanmodel.h \
        imageProviders/abstractimageprovider.h \
        datamodels/DataModelItems/sleepingalarm.h \
        datamodels/avcodegrid.h \
        datamodels/floorplanimageitem.h \
        contextobjects/floorplandevice.h \
        contextobjects/screenshotattributes.h \
        orbiterwindow.h \
        contextobjects/screensaverclass.h \
        datamodels/skinloader.h  \
        datamodels/genericsetupmodel.h \
        datamodels/genericsetupitem.h \
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
        contextobjects/promptdata.h \
        datamodels/existingorbitermodel.h \
        datamodels/devicemodel.h \
        datamodels/attributeobject.h \
        logger/qorbiterlogger.h \
        datamodels/genericflatlistmodel.h \
        datamodels/genericmodelitem.h \
        datamodels/ActiveMediaStreams.h \
        datamodels/MediaFilter.h \
        datamodels/DataGridHandler.h \
        datamodels/listitembase.h \
        defineObjects/linuxmcedata.h \
        defineObjects/mediatypehelper.h \
        RemoteCommands.h \
        defineObjects/datagrids.h \
    contextobjects/screeninfo.h \
    contextobjects/settinginterface.h \
    contextobjects/entertainareatimecode.h \
    DCECommand.h \
    datamodels/diskmediaitem.h \
    managerHelpers/taskshelper.h \
    managerHelpers/dcemediahelper.h \
    managerHelpers/routerhelper.h



OTHER_FILES += Readme.txt \
        OrbiterVariables.txt \
        config.xml

for_harmattan{
        OTHER_FILES= \
        qtc_packaging/debian_harmattan/rules \
        qtc_packaging/debian_harmattan/README \
        qtc_packaging/debian_harmattan/copyright \
        qtc_packaging/debian_harmattan/control \
        qtc_packaging/debian_harmattan/compat \
        qtc_packaging/debian_harmattan/changelog \
}

DISTFILES += \
    qml/skins/default/components/FloorplanOnOff.qml \
    qml/skins/default/components/FloorplanFanSwitch.qml \
    qml/skins/default/components/FloorplanSetTemp.qml \
    qml/skins/default/components/FloorplanHeatCool.qml \
    qml/skins/default/components/FloorplanDefaultControl.qml \
    ../docs/RPI-Dev-Setup.txt \
    qml/skins/default/components/NowPlayingTransferMedia.qml \
    ../buildScripts/rpi/runSingleQorbiter.sh








