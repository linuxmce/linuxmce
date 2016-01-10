# Microsoft Developer Studio Project File - Name="PocketFrog" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=PocketFrog - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PocketFrog.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PocketFrog.mak" CFG="PocketFrog - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PocketFrog - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "PocketFrog - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PocketFrog - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../lib/"
# PROP Intermediate_Dir "../obj/x86/release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/PocketFrog_x86.lib"

!ELSEIF  "$(CFG)" == "PocketFrog - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib/"
# PROP Intermediate_Dir "../obj/x86/debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/PocketFrog_x86_Debug.lib"

!ENDIF 

# Begin Target

# Name "PocketFrog - Win32 Release"
# Name "PocketFrog - Win32 Debug"
# Begin Group "Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\config.h
# End Source File
# Begin Source File

SOURCE=..\src\defs.h
# End Source File
# Begin Source File

SOURCE=..\src\display.cpp
# End Source File
# Begin Source File

SOURCE=..\src\display.h
# End Source File
# Begin Source File

SOURCE=..\src\image.cpp
# End Source File
# Begin Source File

SOURCE=..\src\image.h
# End Source File
# Begin Source File

SOURCE=..\src\input.cpp
# End Source File
# Begin Source File

SOURCE=..\src\input.h
# End Source File
# Begin Source File

SOURCE=..\PocketFrog.h
# End Source File
# Begin Source File

SOURCE=..\src\pocketpc.cpp
# End Source File
# Begin Source File

SOURCE=..\src\pocketpc.h
# End Source File
# Begin Source File

SOURCE=..\src\rasterizer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\rasterizer.h
# End Source File
# Begin Source File

SOURCE=..\src\rect.cpp
# End Source File
# Begin Source File

SOURCE=..\src\rect.h
# End Source File
# Begin Source File

SOURCE=..\src\surface.cpp
# End Source File
# Begin Source File

SOURCE=..\src\surface.h
# End Source File
# End Group
# Begin Group "Internal"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\internal\algo2d.h
# End Source File
# Begin Source File

SOURCE=..\src\internal\blit16.cpp
# End Source File
# Begin Source File

SOURCE=..\src\internal\blit4.cpp
# End Source File
# Begin Source File

SOURCE=..\src\internal\blit8.cpp
# End Source File
# Begin Source File

SOURCE=..\src\internal\displaydriver.h
# End Source File
# Begin Source File

SOURCE=..\src\internal\driver.h
# End Source File
# Begin Source File

SOURCE=..\src\internal\GAPILibrary.cpp
# End Source File
# Begin Source File

SOURCE=..\src\internal\GAPILibrary.h
# End Source File
# Begin Source File

SOURCE=..\src\internal\graphicbuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\internal\graphicbuffer.h
# End Source File
# Begin Source File

SOURCE=..\src\internal\inputdriver.h
# End Source File
# Begin Source File

SOURCE=..\src\internal\memfile.cpp
# End Source File
# Begin Source File

SOURCE=..\src\internal\memfile.h
# End Source File
# Begin Source File

SOURCE=..\src\internal\raster2d.cpp
# End Source File
# Begin Source File

SOURCE=..\src\internal\raster2d.h
# End Source File
# Begin Source File

SOURCE=..\src\internal\Win32Drivers.cpp
# End Source File
# Begin Source File

SOURCE=..\src\internal\Win32Drivers.h
# End Source File
# End Group
# Begin Group "Framework"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\framework\game.cpp
# End Source File
# Begin Source File

SOURCE=..\src\framework\game.h
# End Source File
# Begin Source File

SOURCE=..\src\framework\splashimg.h
# End Source File
# Begin Source File

SOURCE=..\src\framework\splashscreen.cpp
# End Source File
# End Group
# Begin Group "Foreign"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\foreign\imgdecmp\imgdecmp.h
# End Source File
# Begin Source File

SOURCE=..\src\foreign\imgdecmp\imgrendr.h
# End Source File
# Begin Source File

SOURCE=..\src\foreign\boost\scoped_array.hpp
# End Source File
# Begin Source File

SOURCE=..\src\foreign\boost\scoped_ptr.hpp
# End Source File
# Begin Source File

SOURCE=..\src\foreign\boost\utility.hpp
# End Source File
# End Group
# Begin Group "Documentation"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\history.txt
# End Source File
# Begin Source File

SOURCE=..\license.txt
# End Source File
# End Group
# End Target
# End Project
