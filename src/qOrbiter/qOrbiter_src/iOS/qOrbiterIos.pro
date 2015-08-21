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

TEMPLATE=app
CONFIG += thread static

# define deployment destination and target executable name
#TARGET=qorbiter



#!macx-clang-ios{  include(../platforms/Android/QmlVlc/QmlVlc/QmlVlc.pri) }

#Translation files
TRANSLATIONS += app_de.ts

#Configuration & Flags
                QT+= qml quick script widgets xml network multimedia core

                LIBS += -L$$[QT_INSTALL_PREFIX]/qml/QtQuick/Controls -lqtquickcontrolsplugin

                folder_01.source = ImportedItems.qml
                folder_01.path =/

                splash.files=../qml/splash/
                splash.path=/

                skins.files=../qml/skins
                skins.path=/

                loading.files=../qml/Index.qml
                loading.path=/

                #QMAKE_BUNDLE_DATA+=ICON


                DEFINES += GLENABLED QT5
                glmsg=yes
                ICON = ../../platforms/ios/assets/icons/*.png
                QMAKE_BUNDLE_DATA+= skins splash ICON
                QMAKE_CXXFLAGS+=-Wno-c++11-narrowing -DUSE_LZO_DATAGRID            
                CONFIG +=warn_off
                QMAKE_INFO_PLIST = QOrbiter.plist
                INSTALLS+= skins splash
                DEPLOYMENTFOLDERS+=folder_01 skins splash
                #APP_RESOURCES_PATH=../../../$$DESTDIR/$$TARGET".app"/Contents/resources
                QML_IMPORT_PATH=imports #../../../$$DESTDIR/$$TARGET".app"/Contents/MacOS/imports

      message("You Must copy the libraries at $$[QT_INSTALL_PREFIX]/qml to $$QTDIR/imports before deploying" )
      message("Target folder::" $$plugins_folder.target)


# Add more folders to ship with the application, here
#common-folder.source = qml/skins-common
#common-folder.target = $$DESTDIR/qml

#DEPLOYMENTFOLDERS+=common-folder


# Additional import path used to resolve QML modules in Creator's code model

QML_IMPORT_TRACE = 1


#LinuxMCE Specific include path. Linking in the app instead of against dce libs for multi-platform expediency.
INCLUDEPATH += ../../../ ../../../../DCE/ ../

#LIBS+=-L$$[QT_INSTALL_IMPORTS]/DceScreenSaver -lDceScreenSaverPlugin
LIBS+=-L$$[QT_INSTALL_IMPORTS]/AudioVisual -lAudioVisualPlugin




message(QOrbiter for iOS  - version: $$[QT_VERSION])
message(Qt is installed in $$[QT_INSTALL_PREFIX])
message (Build Definitions: $$DEFINES)
message( Opengl Status: $$glmsg )
message( Target import path: $$[QT_INSTALL_IMPORTS])
message( Binary name: $$TARGET )
message( Output Path $$DESTDIR )
message ( Plist file::$$QMAKE_INFO_PLIST )

SOURCES += ../main.cpp \
        ../../qOrbiter.cpp \
        ../../../Gen_Devices/qOrbiterBase.cpp \
        ../qorbitermanager.cpp \
        ../datamodels/listModel.cpp \
        ../datamodels/gridItem.cpp \
        ../imageProviders/gridimageprovider.cpp \
        ../imageProviders/basicImageProvider.cpp \
        ../datamodels/locationmodel.cpp \
        ../datamodels/locationitem.cpp \
        ../datamodels/usermodel.cpp \
        ../datamodels/useritem.cpp \
        ../datamodels/lightingscenariomodel.cpp \
        ../datamodels/lightingscenarioitem.cpp \
        ../datamodels/mediascenariomodel.cpp \
        ../datamodels/mediascenarioitem.cpp \
        ../datamodels/telecomscenariomodel.cpp \
        ../datamodels/climatescenariomodel.cpp \
        ../datamodels/securityscenariomodel.cpp \
        ../datamodels/climatescenarioitem.cpp \
        ../datamodels/securityscenarioitem.cpp \
        ../datamodels/telecomscenarioitem.cpp \
        ../screensaver/screensavermodule.cpp \
        ../../../PlutoUtils/uuencode.cpp \
        ../../../PlutoUtils/ThreadedClass.cpp \
        ../../../PlutoUtils/Other.cpp \
        ../../../PlutoUtils/MultiThreadIncludes.cpp \
        ../../../PlutoUtils/minilzo.cpp \
        ../../../PlutoUtils/md5c.cpp \
        ../../../PlutoUtils/FileUtils.cpp \
        ../../../PlutoUtils/CommonIncludes.cpp \
        ../../../SerializeClass/SerializeClass.cpp \
        ../../../DCE/Virtual_Device_Translator.cpp \
        ../../../DCE/Socket.cpp \
        ../../../DCE/ServerLogger.cpp \
        ../../../DCE/PlainClientSocket.cpp \
        ../../../DCE/MessageBuffer.cpp \
        ../../../DCE/Message.cpp \
        ../../../DCE/HandleRequestSocket.cpp \
        ../../../DCE/Logger.cpp \
        ../../../DCE/Event_Impl.cpp \
        ../../../DCE/DCEConfig.cpp \
        ../../../DCE/DataGrid.cpp \
        ../../../DCE/Command_Impl.cpp \
        ../../../DCE/AlarmManager.cpp \
        ../../../PlutoUtils/StringUtils.cpp \
        ../../../DCE/ClientSocket.cpp \
        ../../../DCE/DeviceData_Base.cpp \
        ../../../DCE/DeviceData_Impl.cpp \
        ../datamodels/skindatamodel.cpp \
        ../datamodels/skindataitem.cpp \
        ../datamodels/DataModelItems/filtermodelitem.cpp \
        ../datamodels/DataModelItems/genreitem.cpp \
        ../../../PlutoUtils/getch.cpp \
        ../datamodels/DataModelItems/attributesortitem.cpp \
        ../datamodels/attributesortmodel.cpp \
        ../datamodels/DataModelItems/mediatypeitem.cpp \
        ../contextobjects/nowplayingclass.cpp \
        ../contextobjects/screenparamsclass.cpp \
        ../contextobjects/playlistclass.cpp \
        ../contextobjects/playlistitemclass.cpp \
        ../contextobjects/securityvideoclass.cpp \
        ../contextobjects/epgchannellist.cpp \
        ../contextobjects/screeninfo.cpp \
        ../playlists/epgitemclass.cpp \
        ../datamodels/floorplanmodel.cpp \
        ../datamodels/floorplanitem.cpp \
        ../imageProviders/abstractimageprovider.cpp \
        ../datamodels/DataModelItems/sleepingalarm.cpp \
        ../contextobjects/filedetailsclass.cpp \
        ../datamodels/avcodegrid.cpp \
        ../contextobjects/floorplandevice.cpp \
        ../contextobjects/screenshotattributes.cpp \
        ../orbiterwindow.cpp \
        ../contextobjects/screensaverclass.cpp \
        ../datamodels/skinloader.cpp \
        ../datamodels/genericsetupmodel.cpp \
        ../datamodels/genericsetupitem.cpp \
        ../datamodels/avdevice.cpp \
        ../contextobjects/existingorbiter.cpp \
        ../contextobjects/modelpage.cpp \
        ../contextobjects/avcommand.cpp \
        ../shaders/filereader.cpp \
        ../contextobjects/timecodemanager.cpp \
        ../imageProviders/floorplandeviceprovider.cpp \
        ../contextobjects/bookmarkitem.cpp \
        ../contextobjects/promptdata.cpp \
        ../datamodels/existingorbitermodel.cpp \
        ../datamodels/devicemodel.cpp \
        ../datamodels/attributeobject.cpp \
        ../logger/qorbiterlogger.cpp \
        ../datamodels/genericflatlistmodel.cpp \
        ../datamodels/genericmodelitem.cpp \
        ../datamodels/ActiveMediaStreams.cpp \
        ../datamodels/listitembase.cpp \
        ../defineObjects/linuxmcedata.cpp \
        ../defineObjects/mediatypehelper.cpp \
        ../defineObjects/datagrids.cpp \
    ../contextobjects/settinginterface.cpp \
    ../contextobjects/entertainareatimecode.cpp

HEADERS += \
        ../../../PlutoUtils/ThreadedClass.h \
        ../../../PlutoUtils/MultiThreadIncludes.h \
        ../../../PlutoUtils/StringUtils.h \
        ../../../PlutoUtils/CommonIncludes.h \
        ../../../PlutoUtils/Other.h \
        ../../../PlutoUtils/getch.h \
        ../../../PlutoUtils/MyStl.h \
        ../../../DCE/DeviceData_Base.h \
        ../../../DCE/Message.h \
        ../../../DCE/ServerLogger.h \
        ../../../DCE/Logger.h \
        ../../../DCE/Virtual_Device_Translator.h \
        ../../../DCE/PlutoLockLogger.h \
        ../../../DCE/ClientSocket.h \
        ../../../DCE/PlainClientSocket.h \
        ../../../DCE/AlarmManager.h \
        ../../../SerializeClass/SerializeClass.h \
        ../../../PlutoUtils/FileUtils.h \
        ../../../pluto_main/Define_DeviceCategory.h \
        ../../../pluto_main/Define_DeviceTemplate.h \
        ../../qOrbiter.h \
        ../../../Gen_Devices/qOrbiterBase.h \
        ../qorbitermanager.h \
        ../qOrbiterData.h \
        ../datamodels/listModel.h \
        ../datamodels/gridItem.h \
        ../imageProviders/gridimageprovider.h \
        ../imageProviders/basicImageProvider.h \
        ../datamodels/locationmodel.h \
        ../datamodels/locationitem.h \
        ../datamodels/usermodel.h \
        ../datamodels/useritem.h \
        ../datamodels/lightingscenariomodel.h \
        ../datamodels/lightingscenarioitem.h \
        ../datamodels/mediascenariomodel.h \
        ../datamodels/mediascenarioitem.h \
        ../datamodels/telecomscenariomodel.h \
        ../datamodels/climatescenariomodel.h \
        ../datamodels/securityscenariomodel.h \
        ../datamodels/climatescenarioitem.h \
        ../datamodels/securityscenarioitem.h \
        ../datamodels/telecomscenarioitem.h \
        ../screensaver/screensavermodule.h \
        ../datamodels/skindatamodel.h \
        ../datamodels/skindataitem.h \
        ../datamodels/DataModelItems/filtermodelitem.h \
        ../datamodels/DataModelItems/genreitem.h \
        ../datamodels/DataModelItems/attributesortitem.h \
        ../datamodels/attributesortmodel.h \
        ../datamodels/DataModelItems/mediatypeitem.h \
        ../contextobjects/filedetailsclass.h \
        ../contextobjects/nowplayingclass.h \
        ../contextobjects/screenparamsclass.h \
        ../contextobjects/playlistclass.h \
        ../contextobjects/playlistitemclass.h \
        ../contextobjects/securityvideoclass.h \
        ../contextobjects/epgchannellist.h \
        ../contextobjects/screeninfo.h \
        ../playlists/epgitemclass.h \
        ../datamodels/floorplanmodel.h \
        ../imageProviders/abstractimageprovider.h \
        ../datamodels/DataModelItems/sleepingalarm.h \
        ../datamodels/avcodegrid.h \
        ../datamodels/floorplanimageitem.h \
        ../contextobjects/floorplandevice.h \
        ../contextobjects/screenshotattributes.h \
        ../orbiterwindow.h \
        ../contextobjects/screensaverclass.h \
        ../datamodels/skinloader.h  \
        ../datamodels/genericsetupmodel.h \
        ../datamodels/genericsetupitem.h \
        ../datamodels/avdevice.h \
        ../contextobjects/existingorbiter.h \
        ../contextobjects/modelpage.h \
        ../contextobjects/avcommand.h \
        ../shaders/filereader.h \
        ../shaders/trace.h \
        ../contextobjects/timecodemanager.h \
        ../imageProviders/floorplandeviceprovider.h \
        ../contextobjects/bookmarkitem.h \
        ../contextobjects/promptdata.h \
        ../datamodels/existingorbitermodel.h \
        ../datamodels/devicemodel.h \
        ../datamodels/attributeobject.h \
        ../logger/qorbiterlogger.h \
        ../datamodels/genericflatlistmodel.h \
        ../datamodels/genericmodelitem.h \
        ../datamodels/ActiveMediaStreams.h \
        ../datamodels/MediaFilter.h \
        ../datamodels/DataGridHandler.h \
        ../datamodels/listitembase.h \
        ../defineObjects/linuxmcedata.h \
        ../defineObjects/mediatypehelper.h \
        ../RemoteCommands.h \
        ../defineObjects/datagrids.h \
        ../../datamodels/DataGridHandler.h \
    ../contextobjects/settinginterface.h \
    ../contextobjects/entertainareatimecode.h



OTHER_FILES += Readme.txt \
        OrbiterVariables.txt \
        config.xml \
    QOrbiter.plist


RESOURCES += \
        ../skinData.qrc \
        ../qmlFile.qrc


include(deployment.pri)

