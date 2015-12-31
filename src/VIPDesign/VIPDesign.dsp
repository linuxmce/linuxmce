# Microsoft Developer Studio Project File - Name="VIPDesign" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=VIPDesign - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VIPDesign.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VIPDesign.mak" CFG="VIPDesign - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VIPDesign - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "VIPDesign - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VIPDesign - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "VIPDesign - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /w /W0 /Gm /GX /ZI /Od /I ".\\" /I "\ha\level4" /I "\ha\level1\Libraries\pthreads-win32\include" /I "\ha\level4\DCE" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "MULTI_THREADED" /D "VIPDESIGN" /D "VIPPHONE" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 C:/ha/level1/Libraries/pthreads-win32/lib/pthreadVC.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../VIPDesign.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "VIPDesign - Win32 Release"
# Name "VIPDesign - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\DCE\AlarmManager.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\BT_CP_Disconnect.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\BT_CP_Have_Nothing.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\BT_CP_SendMeKeystrokes.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\BT_CP_ShowImage.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\BT_CP_ShowVMC.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\BT_CP_UpdateVariables.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\BT_PC_Disconnect.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\BT_PC_KeyWasPressed.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\BT_PC_OrderPlaced.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\BT_PC_ReportingVariables.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\BT_PC_VMCHidden.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\BT_PC_VMCTerminated.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\BT_PC_WhatDoYouHave.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\BTCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\BTCommandProcessor.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\BTCommandProcessor_Windows_Socket.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=..\DCE\CommonIncludes.cpp
# End Source File
# Begin Source File

SOURCE=..\DCE\DCEClientSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\DCE\DCELogger.cpp
# End Source File
# Begin Source File

SOURCE=..\DCE\DCEMessage.cpp
# End Source File
# Begin Source File

SOURCE=..\DCE\DCEServerSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\DCE\DCESocket.cpp
# End Source File
# Begin Source File

SOURCE=..\DCE\DCESocketListener.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPEstablishment\EstablishmentSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=..\DCE\MultiThreadIncludes.cpp
# End Source File
# Begin Source File

SOURCE=..\DCE\Picture.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\PlutoConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\RenderMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\ResetEmulatorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\DCE\StringUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\DCE\TokenStream.cpp
# End Source File
# Begin Source File

SOURCE=.\VIPDesign.cpp
# End Source File
# Begin Source File

SOURCE=.\VIPDesign.rc
# End Source File
# Begin Source File

SOURCE=.\VIPDesignDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\VIPDesignView.cpp
# End Source File
# Begin Source File

SOURCE=.\VIPEditDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VIPEmulator.cpp
# End Source File
# Begin Source File

SOURCE=.\VIPEmulatorSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\VIPGraphicDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VIPInputDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VIPListDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\VIPMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\VIPMenuDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VIPPropertiesDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\VIPRequest.cpp
# End Source File
# Begin Source File

SOURCE=.\VIPResolutionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VIPTextDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VIPVariableDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\VR_IdentifyPhone.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\VR_ManuallyMatchPhone.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\VR_PhoneRespondedToRequest.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\VR_RequestPayment.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\VR_RequestSecureTransaction.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\VR_ShowMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\VIPShared\VR_UnknownRequest.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Customer.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\VIPDesign.h
# End Source File
# Begin Source File

SOURCE=.\VIPDesignDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\VIPDesign.ico
# End Source File
# Begin Source File

SOURCE=.\res\VIPDesign.rc2
# End Source File
# Begin Source File

SOURCE=.\res\VIPDesignDoc.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\res\VIPDesign.manifest
# End Source File
# End Target
# End Project
