# Add more folders to ship with the application, here
folder_01.source = qml/qObiter_src
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

symbian:TARGET.UID3 = 0xE0D07D4D

INCLUDEPATH += ../../ ../../DCE/

LIBS += -L/usr/pluto/lib/ -lDCECommon -lSerializeClass -lPlutoUtils -lpluto_main -lpluto_media

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
    qOrbiterGen/qorbitergenerator.cpp \
    datamodels/orbitermodel.cpp \
    datamodels/orbiteritem.cpp \
    datamodels/listModel.cpp \
    datamodels/gridItem.cpp \
    imageProviders/gridimageprovider.cpp \
    imageProviders/basicImageProvider.cpp

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

HEADERS += \
    ../qOrbiter.h \
    ../../Gen_Devices/qOrbiterBase.h \
    uiclasses/lightingui.h \
    uiclasses/uicontroller.h \
    qorbitermanager.h \
    listModel.h \
    gridItem.h \
    gridimageprovider.h \
    basicImageProvider.h \
    qorbitergenerator.h \
    orbitermodel.h \
    orbiteritem.h \
    qOrbiterData.h \
    qOrbiterGen/qorbitergenerator.h \
    datamodels/orbitermodel.h \
    datamodels/orbiteritem.h \
    datamodels/listModel.h \
    datamodels/gridItem.h \
    imageProviders/gridimageprovider.h \
    imageProviders/basicImageProvider.h

OTHER_FILES += \
    qml/skins/scheme/classic/Main.qml \
    img/lmcesplash.jpg \
    qml/skins/scheme/classic/StyleClassic.qml \
    qml/skins/scheme/brown/StyleBrown.qml \
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
    img/icons/agt_games.png

RESOURCES += \
    skinData.qrc
