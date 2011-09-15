# Add more folders to ship with the application, here

for_desktop{
folder_01.source = qml/desktop
folder_01.target = qml

folder_02.source= img
folder_02.target=     #left blank so it will appear in the root
DEFINES += for_desktop
}

for_freemantle{
folder_01.source = qml/freemantle
folder_01.target = qml

folder_02.source= img
folder_02.target=     #left blank so it will appear in the root
DEFINES += for_freemantle
}

for_harmattan{
folder_01.source = qml/harmattan
folder_01.target = qml

folder_02.source= img
folder_02.target=     #left blank so it will appear in the root
DEFINES += for_harmattan
}

macx{
folder_01.source = qml/desktop
folder_01.target = qml

folder_02.source= img
folder_02.target=     #left blank so it will appear in the root

}

ANDROID{
folder_01.source = qml/desktop
folder_01.target = qml

folder_02.source= img
folder_02.target=     #left blank so it will appear in the root
DEFINES +=ANDROID
}

folder_03.source = config.xml
folder_03.TARGET =
DEPLOYMENTFOLDERS = folder_01 folder_02 folder_03
DESTDIR=../build-output
TARGET = qOrbiter
# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =
symbian:TARGET.UID3 = 0xE0D07D4D
QMAKE_CXXFLAGS += -DUSE_LZO_DATAGRID
INCLUDEPATH += ../../ ../../DCE/

macx{
    QT += xml
    TARGET = qOrbiter
    ICON = osxicons.icns
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
    uiclasses/lightingui.cpp \
    uiclasses/uicontroller.cpp \
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
    contextobjects/filedetailsclass.cpp \
    imageProviders/filedetailsimageprovider.cpp \
    contextobjects/nowplayingclass.cpp \
    datamodels/DatagridClasses/datagriditemmodelclass.cpp \
    contextobjects/screenparamsclass.cpp \
    contextobjects/playlistclass.cpp \
    contextobjects/playlistitemclass.cpp \
    datamodels/DatagridClasses/datagriditem.cpp \
    imageProviders/nowplayingimageprovider.cpp \
    contextobjects/securityvideoclass.cpp \
    imageProviders/securityvideoimage.cpp \
    contextobjects/epgchannellist.cpp \
    playlists/epgitemclass.cpp

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

HEADERS += \
    ../qOrbiter.h \
    ../../Gen_Devices/qOrbiterBase.h \
    uiclasses/lightingui.h \
    uiclasses/uicontroller.h \
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
    imageProviders/filedetailsimageprovider.h \
    contextobjects/nowplayingclass.h \
    datamodels/DatagridClasses/datagriditemmodelclass.h \
    contextobjects/screenparamsclass.h \
    contextobjects/playlistclass.h \
    contextobjects/playlistitemclass.h \
    datamodels/DatagridClasses/datagriditem.h \
    imageProviders/nowplayingimageprovider.h \
    contextobjects/securityvideoclass.h \
    imageProviders/securityvideoimage.h \
    contextobjects/epgchannellist.h \
    playlists/epgitemclass.h

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
    android/AndroidManifest.xml

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










