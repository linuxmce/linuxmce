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
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I ".." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\PocketFrog_x86.lib"

!ELSEIF  "$(CFG)" == "PocketFrog - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\PocketFrog_x86_Debug.lib"

!ENDIF 

# Begin Target

# Name "PocketFrog - Win32 Release"
# Name "PocketFrog - Win32 Debug"
# Begin Group "Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\config.h
# End Source File
# Begin Source File

SOURCE=..\source\defs.h
# End Source File
# Begin Source File

SOURCE=..\source\display.cpp
# End Source File
# Begin Source File

SOURCE=..\source\display.h
# End Source File
# Begin Source File

SOURCE=..\source\image.cpp
# End Source File
# Begin Source File

SOURCE=..\source\image.h
# End Source File
# Begin Source File

SOURCE=..\PocketFrog\PocketFrog.h
# End Source File
# Begin Source File

SOURCE=..\source\pocketpc.cpp
# End Source File
# Begin Source File

SOURCE=..\source\pocketpc.h
# End Source File
# Begin Source File

SOURCE=..\source\rasterizer.cpp
# End Source File
# Begin Source File

SOURCE=..\source\rasterizer.h
# End Source File
# Begin Source File

SOURCE=..\source\rect.cpp
# End Source File
# Begin Source File

SOURCE=..\source\rect.h
# End Source File
# Begin Source File

SOURCE=..\source\surface.cpp
# End Source File
# Begin Source File

SOURCE=..\source\surface.h
# End Source File
# End Group
# Begin Group "Internal"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\internal\algo2d.h
# End Source File
# Begin Source File

SOURCE=..\source\internal\memfile.cpp
# End Source File
# Begin Source File

SOURCE=..\source\internal\memfile.h
# End Source File
# Begin Source File

SOURCE=..\source\internal\raster2d.cpp
# End Source File
# Begin Source File

SOURCE=..\source\internal\raster2d.h
# End Source File
# End Group
# Begin Group "Framework"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\framework\game.cpp
# End Source File
# Begin Source File

SOURCE=..\source\framework\game.h
# End Source File
# End Group
# Begin Group "Foreign"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\foreign\imgdecmp\imgdecmp.h
# End Source File
# Begin Source File

SOURCE=..\source\foreign\imgdecmp\imgrendr.h
# End Source File
# Begin Source File

SOURCE=..\source\foreign\boost\scoped_array.hpp
# End Source File
# Begin Source File

SOURCE=..\source\foreign\boost\scoped_ptr.hpp
# End Source File
# Begin Source File

SOURCE=..\source\foreign\boost\utility.hpp
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
