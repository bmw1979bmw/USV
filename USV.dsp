# Microsoft Developer Studio Project File - Name="USV" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=USV - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "USV.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "USV.mak" CFG="USV - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "USV - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "USV - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "USV - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W4 /GX /O2 /I "..\shared" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "USE_FMX" /D WINVER=0x501 /D _WIN32_WINNT=0x501 /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /i "..\shared" /i "." /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# Begin Special Build Tool
TargetPath=.\Release\USV.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=Copy $(TargetPath) d:\stand9demo
# End Special Build Tool

!ELSEIF  "$(CFG)" == "USV - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /I "..\shared" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "DEMO" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /i "..\shared" /i "." /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=.\Debug\USV.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=Copy $(TargetPath) d:\stand9demo
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "USV - Win32 Release"
# Name "USV - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=D:\project\Shared\CmdLine.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\FMExchange.cpp
# End Source File
# Begin Source File

SOURCE=.\FormularDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Functions.cpp
# End Source File
# Begin Source File

SOURCE=D:\project\Shared\GoodFunc.cpp
# End Source File
# Begin Source File

SOURCE=.\MainDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MainView.cpp
# End Source File
# Begin Source File

SOURCE=D:\project\Shared\math2.cpp
# End Source File
# Begin Source File

SOURCE=D:\project\Shared\RegsKey.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\StandApp.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\StandFile.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=D:\project\Shared\TextArchive.cpp
# End Source File
# Begin Source File

SOURCE=D:\project\Shared\TXParamsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\USV.cpp
# End Source File
# Begin Source File

SOURCE=.\USV.rc
# End Source File
# Begin Source File

SOURCE=D:\project\Shared\WorldClient.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\WsgFile.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=D:\project\Shared\CmdLine.h
# End Source File
# Begin Source File

SOURCE=.\FormularDlg.h
# End Source File
# Begin Source File

SOURCE=.\Functions.h
# End Source File
# Begin Source File

SOURCE=D:\project\Shared\GoodFunc.h
# End Source File
# Begin Source File

SOURCE=.\MainDoc.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MainView.h
# End Source File
# Begin Source File

SOURCE=D:\project\Shared\math2.h
# End Source File
# Begin Source File

SOURCE=D:\project\Shared\RegsKey.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=D:\project\Shared\TextArchive.h
# End Source File
# Begin Source File

SOURCE=D:\project\Shared\TXParamsDlg.h
# End Source File
# Begin Source File

SOURCE=.\USV.h
# End Source File
# Begin Source File

SOURCE=D:\project\Shared\WorldClient.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\idr_USV.ico
# End Source File
# Begin Source File

SOURCE=.\res\MainDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\sysidsel.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\USV.ico
# End Source File
# Begin Source File

SOURCE=.\res\USV.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
