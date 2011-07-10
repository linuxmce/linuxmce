TEMPLATE = app
QT = gui core network xml
CONFIG += qt debug warn_on console
DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
FORMS = ui/mainwindow.ui
HEADERS = src/mainwindowimpl.h \
 src/InteractiveQLabel.h \
 src/OrbiterBackend.h \
 src/AppData.h \
 src/OrbiterScreen.h \
 src/DatagridWidget.h
SOURCES = src/mainwindowimpl.cpp \
 src/main.cpp \
 src/InteractiveQLabel.cpp \
 src/OrbiterBackend.cpp \
 src/OrbiterScreen.cpp \
 src/DatagridWidget.cpp
