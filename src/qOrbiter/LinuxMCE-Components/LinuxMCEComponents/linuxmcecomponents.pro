TEMPLATE = lib
TARGET = UIElements
QT += declarative
CONFIG += qt plugin

uri = Linuxmce.UIElements

OTHER_FILES = qmldir

# Input
SOURCES += \
    button.cpp \
    uielements_plugin.cpp \
    screensaver.cpp

HEADERS += \
    button.h \
    uielements_plugin.h \
    screensaver.h

DESTDIR = ../../qOrbiter_src/imports/Linuxmce/UIElements

message($$_PRO_FILE_PWD_)
message($$OUT_PWD)
    copy_qmldir.target = $$DESTDIR/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target

unix {
    maemo5 | !isEmpty(MEEGO_VERSION_MAJOR) {
        installPath = /usr/lib/qt4/imports/$$replace(uri, \\., /)
    } else {
        installPath = $$[QT_INSTALL_IMPORTS]/$$replace(uri, \\., /)
    }
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}

