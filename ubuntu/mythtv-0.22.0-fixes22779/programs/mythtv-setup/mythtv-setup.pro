include ( ../../settings.pro )
include ( ../programs-libs.pro )

QT += network xml sql

INCLUDEPATH += ../../libs/libmythtv/channelscan
DEPENDPATH += ../../libs/libmythtv/channelscan

TEMPLATE = app
CONFIG += thread
TARGET = mythtv-setup
target.path = $${PREFIX}/bin

INSTALLS = target

menu.path = $${PREFIX}/share/mythtv/
menu.files += setup.xml

INSTALLS += menu

QMAKE_CLEAN += $(TARGET)

using_backend {
    DEFINES += USING_BACKEND
}

# Input
HEADERS += backendsettings.h   channeleditor.h   checksetup.h
HEADERS += exitprompt.h        importicons.h     startprompt.h
SOURCES += backendsettings.cpp channeleditor.cpp checksetup.cpp
SOURCES += exitprompt.cpp      importicons.cpp   startprompt.cpp
SOURCES += main.cpp

macx {
    mac_bundle {
        QMAKE_POST_LINK = ../../contrib/OSX/build/makebundle.sh mythtv-setup
    }
}

using_x11:DEFINES += USING_X11
