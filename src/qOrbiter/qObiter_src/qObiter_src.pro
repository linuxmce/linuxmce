# Add more folders to ship with the application, here
folder_01.source = qml/qObiter_src
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

symbian:TARGET.UID3 = 0xE0D07D4D

QMAKE_CXXFLAGS += -DUSE_LZO_DATAGRID

INCLUDEPATH += ../../ ../../DCE/ /usr/include/mysql/

LIBS += -lQtXml

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
    ../../PlutoUtils/ProcessUtils.cpp \
    ../../PlutoUtils/Other.cpp \
    ../../PlutoUtils/MultiThreadIncludes.cpp \
    ../../PlutoUtils/minilzo.cpp \
    ../../PlutoUtils/md5c.cpp \
    ../../PlutoUtils/LinuxSerialUSB.cpp \
    ../../PlutoUtils/getch.cpp \
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
    ../../DCE/DeviceData_Impl.cpp

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
    screensaver/screensavermodule.h

OTHER_FILES += \
    qml/skins/scheme/classic/Main.qml \
    img/lmcesplash.jpg \
    qml/skins/scheme/classic/StyleClassic.qml \
    img/UnknownUser.png \
    qml/thom/screentest.qml \
    qml/thom/screentest_images/new_layer.png \
    qml/thom/screentest_images/new_layer__2.png \
    qml/thom/screentest_images/new_layer__1.png \
    qml/thom/screentest_images/lightingmediaclimate.png \
    qml/thom/screentest_images/glow.png \
    qml/thom/screentest_images/foo.png \
    qml/thom/screentest_images/bar.png \
    qml/thom/screentest_images/background.png \
    img/icons/xine.png \
    img/icons/tv.png \
    img/icons/sms_protocol.png \
    img/icons/scifi.png \
    img/icons/player-end.png \
    img/icons/player_stop.png \
    img/icons/player_start.png \
    img/icons/player_rew.png \
    img/icons/player_play.png \
    img/icons/player_pause.png \
    img/icons/player_fwd.png \
    img/icons/player_end1.png \
    img/icons/player_end.png \
    img/icons/player_eject.png \
    img/icons/personal.png \
    img/icons/password.png \
    img/icons/package_games.png \
    img/icons/package_games_arcade.png \
    img/icons/package_editors.png \
    img/icons/Mythtv.png \
    img/icons/lsongs.png \
    img/icons/lphoto.png \
    img/icons/linphone.png \
    img/icons/kweather.png \
    img/icons/ktip.png \
    img/icons/kservices.png \
    img/icons/kmix.png \
    img/icons/kfm_home.png \
    img/icons/kedit.png \
    img/icons/jabber_protocol.png \
    img/icons/harddrive2.png \
    img/icons/gpgsm.png \
    img/icons/fortress.png \
    img/icons/audio&video.png \
    img/icons/aktion.png \
    img/icons/agt_multimedia.png \
    img/icons/agt_mp3.png \
    img/icons/agt_home.png \
    img/icons/agt_games.png \
    img/icons/On.png \
    img/icons/Off.png \
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
    qml/skins/scheme/brown/Style.qml \
    qml/Main.qml \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog

RESOURCES += \
    skinData.qrc
