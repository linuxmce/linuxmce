# Add more folders to ship with the application, here
folder_01.source = qml/Light_Switch
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

symbian:TARGET.UID3 = 0xE5977FB2

# Allow network access on Symbian
symbian:TARGET.CAPABILITY += NetworkServices

# Define QMLJSDEBUGGER to allow debugging of QML in debug builds
# (This might significantly increase build time)
# DEFINES += QMLJSDEBUGGER

# If your application uses the Qt Mobility libraries, uncomment
# the following lines and add the respective components to the 
# MOBILITY variable. 
# CONFIG += mobility
# MOBILITY +=


# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    switchmanager.cpp \
    ../QML_Light_Switch.cpp \ 
    ../../Gen_Devices/QML_Light_SwitchBase.cpp \
    qtlightswitch.cpp

INCLUDEPATH += ../../ ../../DCE/ 

LIBS += -L/usr/pluto/lib/ -lDCECommon -lSerializeClass -lPlutoUtils -lpluto_main -lSerial

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

RESOURCES += \
    Light_Switch_Graphics.qrc

OTHER_FILES += \
    qml/Light_Switch/SwitchButton.qml

HEADERS += \
    switchmanager.h \
    qtlightswitch.h
