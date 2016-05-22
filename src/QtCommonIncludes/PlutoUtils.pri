#This is the a common file used to make keeping track of required dce includes for static linking easier

HEADERS += \
    $$PWD/../PlutoUtils/CommonIncludes.h \
    $$PWD/../PlutoUtils/ConsoleColor.h \
    $$PWD/../PlutoUtils/DBHelper.h \
    $$PWD/../PlutoUtils/FileTransfer.h \
    $$PWD/../PlutoUtils/FileUtils.h \
    $$PWD/../PlutoUtils/getch.h \
    $$PWD/../PlutoUtils/GraphicFormat.h \
    $$PWD/../PlutoUtils/LinuxSerialUSB.h \
    $$PWD/../PlutoUtils/LinuxSoundcard.h \
    $$PWD/../PlutoUtils/lzoconf.h \
    $$PWD/../PlutoUtils/lzodefs.h \
    $$PWD/../PlutoUtils/md5.h \
    $$PWD/../PlutoUtils/minilzo.h \
    $$PWD/../PlutoUtils/MultiThreadIncludes.h \
    $$PWD/../PlutoUtils/MyMap.h \
    $$PWD/../PlutoUtils/MyStl.h \
    $$PWD/../PlutoUtils/Other.h \
    $$PWD/../PlutoUtils/PlutoDefs.h \
    $$PWD/../PlutoUtils/PlutoHelpers.h \
    $$PWD/../PlutoUtils/ProcessUtils.h \
    $$PWD/../PlutoUtils/Profiler.h \
    $$PWD/../PlutoUtils/property.h \
    $$PWD/../PlutoUtils/StringUtils.h \
    $$PWD/../PlutoUtils/ThreadedClass.h \
    $$PWD/../PlutoUtils/threads.h \
    $$PWD/../PlutoUtils/uuencode.h \
    $$PWD/../PlutoUtils/typetraits.h

SOURCES += \
    $$PWD/../PlutoUtils/CommonIncludes.cpp \
    $$PWD/../PlutoUtils/FileUtils.cpp \
    $$PWD/../PlutoUtils/getch.cpp \
    $$PWD/../PlutoUtils/LinuxSerialUSB.cpp \
    $$PWD/../PlutoUtils/LinuxSoundcard.cpp \
    $$PWD/../PlutoUtils/md5c.cpp \
    $$PWD/../PlutoUtils/minilzo.cpp \
    $$PWD/../PlutoUtils/MultiThreadIncludes.cpp \
    $$PWD/../PlutoUtils/Other.cpp \
    $$PWD/../PlutoUtils/ProcessUtils.cpp \
    $$PWD/../PlutoUtils/StringUtils.cpp \
    $$PWD/../PlutoUtils/ThreadedClass.cpp \
    $$PWD/../PlutoUtils/uuencode.cpp

