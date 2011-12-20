 #
#This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org
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


# define deployment destination and target executable name

TARGET = qorbiter

# Add more folders to ship with the application, here
for_desktop{

folder_01.source = qml/desktop
folder_01.target = $$DESTDIR/qml

folder_02.source= img
folder_02.target= $$DESTDIR    #left blank so it will appear in the root
DEFINES += for_desktop
}

for_droid{
folder_01.source = qml/android/phone
folder_01.target = $$DESTDIR/qml

folder_02.source= img
folder_02.target=     #left blank so it will appear in the root
DEFINES += for_droid
}

for_freemantle{

folder_01.source = qml/freemantle
folder_01.target = $$DESTDIR/qml

folder_02.source= img
folder_02.target=     #left blank so it will appear in the root
DEFINES += for_freemantle
}

for_harmattan{
folder_01.source = qml/harmattan
folder_01.target =

folder_02.source= img
folder_02.target=     #left blank so it will appear in the root
DEFINES += for_harmattan
}

macx{

    APP_RESOURCES_PATH=../../../$$DESTDIR/$$TARGET".app"/Contents/resources

    folder_01.source = qml/desktop
    folder_01.target = $$APP_RESOURCES_PATH/qml

    folder_02.source= img
    folder_02.target= $$APP_RESOURCES_PATH   #left blank so it will appear in the root

    folder_03.source = config.xml
    folder_03.target = $$APP_RESOURCES_PATH

    ICON = osxicons.icns
}

ANDROID{
folder_01.source = qml/desktop
folder_01.target = $$DESTDIR/qml

folder_02.source= img
folder_02.target=     #left blank so it will appear in the root
DEFINES +=ANDROID
}

!macx{
    folder_03.source = config.xml
    folder_03.target = $$DESTDIR
}

DEPLOYMENTFOLDERS = folder_01 folder_02 folder_03
# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =
symbian:TARGET.UID3 = 0xE0D07D4D
QMAKE_CXXFLAGS += -DUSE_LZO_DATAGRID
INCLUDEPATH += ../../ ../../DCE/

macx{
    QT += xml
    QT += network
}

!macx{
    LIBS += -lQtXml
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

# The .cpp file which was generated for your project. Feel free to hack it.
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
    contextobjects/screensaverclass.cpp

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

HEADERS += \
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
    ../../PlutoUtils/ThreadedClass.h \
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
    contextobjects/screensaverclass.h

OTHER_FILES += Readme.txt \
    config.xml \
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable-mdpi/icon.png \
    android/res/drawable-ldpi/icon.png \
    android/src/eu/licentia/necessitas/industrius/QtActivity.java \
    android/src/eu/licentia/necessitas/industrius/QtApplication.java \
    android/src/eu/licentia/necessitas/industrius/QtSurface.java \
    android/src/eu/licentia/necessitas/industrius/QtLayout.java \
    android/src/eu/licentia/necessitas/ministro/IMinistroCallback.aidl \
    android/src/eu/licentia/necessitas/ministro/IMinistro.aidl \
    android/src/eu/licentia/necessitas/mobile/QtAndroidContacts.java \
    android/src/eu/licentia/necessitas/mobile/QtSystemInfo.java \
    android/src/eu/licentia/necessitas/mobile/QtFeedback.java \
    android/src/eu/licentia/necessitas/mobile/QtSensors.java \
    android/src/eu/licentia/necessitas/mobile/QtCamera.java \
    android/src/eu/licentia/necessitas/mobile/QtMediaPlayer.java \
    android/src/eu/licentia/necessitas/mobile/QtLocation.java \
    android/AndroidManifest.xml \
    android/res/drawable-mdpi/icon.png \
    android/res/values/strings.xml \
    android/res/values/libs.xml \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable-ldpi/icon.png \
    android/src/eu/licentia/necessitas/mobile/QtSystemInfo.java \
    android/src/eu/licentia/necessitas/mobile/QtLocation.java \
    android/src/eu/licentia/necessitas/mobile/QtCamera.java \
    android/src/eu/licentia/necessitas/mobile/QtAndroidContacts.java \
    android/src/eu/licentia/necessitas/mobile/QtSensors.java \
    android/src/eu/licentia/necessitas/mobile/QtFeedback.java \
    android/src/eu/licentia/necessitas/mobile/QtMediaPlayer.java \
    android/src/eu/licentia/necessitas/industrius/QtSurface.java \
    android/src/eu/licentia/necessitas/industrius/QtLayout.java \
    android/src/eu/licentia/necessitas/industrius/QtApplication.java \
    android/src/eu/licentia/necessitas/industrius/QtActivity.java \
    android/src/eu/licentia/necessitas/ministro/IMinistroCallback.aidl \
    android/src/eu/licentia/necessitas/ministro/IMinistro.aidl \
    android/AndroidManifest.xml \
    qml/Splash.qml \
    qml/SetupNewOrbiter.qml \
    OrbiterVariables.txt \
    qml/harmattan/js/nowPlayingWorker.js

ANDROID{
OTHER_FILES=\
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable-mdpi/icon.png \
    android/res/drawable-ldpi/icon.png \
    android/src/eu/licentia/necessitas/industrius/QtActivity.java \
    android/src/eu/licentia/necessitas/industrius/QtApplication.java \
    android/src/eu/licentia/necessitas/industrius/QtSurface.java \
    android/src/eu/licentia/necessitas/industrius/QtLayout.java \
    android/src/eu/licentia/necessitas/ministro/IMinistroCallback.aidl \
    android/src/eu/licentia/necessitas/ministro/IMinistro.aidl \
    android/src/eu/licentia/necessitas/mobile/QtAndroidContacts.java \
    android/src/eu/licentia/necessitas/mobile/QtSystemInfo.java \
    android/src/eu/licentia/necessitas/mobile/QtFeedback.java \
    android/src/eu/licentia/necessitas/mobile/QtSensors.java \
    android/src/eu/licentia/necessitas/mobile/QtCamera.java \
    android/src/eu/licentia/necessitas/mobile/QtMediaPlayer.java \
    android/src/eu/licentia/necessitas/mobile/QtLocation.java \
    android/AndroidManifest.xml \

}
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




















































contains(MEEGO_EDITION,harmattan) {
    desktopfile.files = $${TARGET}.desktop
    desktopfile.path = /usr/share/applications
    INSTALLS += desktopfile
}




