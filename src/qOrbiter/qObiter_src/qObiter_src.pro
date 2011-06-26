# Add more folders to ship with the application, here
folder_01.source = qml/qObiter_src
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

symbian:TARGET.UID3 = 0xE0D07D4D

INCLUDEPATH += ../../ ../../DCE/

LIBS += -L/usr/pluto/lib/ -lDCECommon -lSerializeClass -lPlutoUtils

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
    listModel.cpp \
    gridItem.cpp \
    gridimageprovider.cpp

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
    basicImageProvider.h

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
    qml/thom/screentest_images/background.png

RESOURCES += \
    skinData.qrc
