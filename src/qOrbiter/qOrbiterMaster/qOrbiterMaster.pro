TEMPLATE = subdirs
DESTDIR = ../build-output
OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \


!macx-ios-clang{
SUBDIRS += ../qOrbiter_src/plugins/AudioVisual \
             ../qOrbiter_src/plugins/DceScreenSaver
}
SUBDIRS += ../qOrbiter_src \




#folder_01.source = ../qOrbiter_src/qml/desktop
#folder_01.target = $$DESTDIR/qml
#folder_02.source= ../qOrbiter_src/img
#folder_02.target= $$DESTDIR   #left blank so it will appear in the root
#folder_03.source = ../qOrbiter_src/qml/template
#folder_03.target = $$DESTDIR/qml
#folder_04.source = ../qOrbiter_src/config.xml
#folder_04.target = $$DESTDIR
#folder_05.source = ../qOrbiter_src/imports
#folder_05.target = $$DESTDIR/imports
#DEPLOYMENTFOLDERS += folder_05



# SUBDIRS = plugins/AudioVisual

