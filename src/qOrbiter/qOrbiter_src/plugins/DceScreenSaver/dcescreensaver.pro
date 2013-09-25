TEMPLATE = lib
TARGET = DceScreenSaver
QT += declarative
CONFIG += qt plugin

contains(QT_VERSION,4.*.*){

message("$$QT_VERSION DCE-ScreenSaver-Plugin")
        QT += declarative opengl
        DEFINES+=QT4
}

contains(QT_VERSION,5.0.*){
        message("$$QT_VERSION DCE-ScreenSaver-Plugin")
        QT += quick1 multimedia network
        DEFINES+=QT5
}

uri = DceScreenSaver

TARGET = $$qtLibraryTarget($$TARGET)
DESTDIR=../../imports/DceScreenSaver
OUT_PWD=DESTDIR
message($$OUT_PWD)
message($$_PRO_FILE_PWD_)
# Input
SOURCES += \
    dcescreensaver_plugin.cpp \
    dcescreensaver.cpp

HEADERS += \
    dcescreensaver_plugin.h \
    dcescreensaver.h

OTHER_FILES = qmldir

!equals($$_PRO_FILE_PWD_, $$DESTDIR) {
    copy_qmldir.target = $$DESTDIR/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
unix {
    maemo5 | !isEmpty(MEEGO_VERSION_MAJOR) {
       installPath = /usr/lib/qt4/imports/$$replace(uri, \\., /)
    } else {
        installPath = $$[QT_INSTALL_IMPORTS]/$$replace(uri, \\., /)
    }
  #  installPath = DESTDIR
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}

