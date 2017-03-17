# Microsoft Developer Studio Project File - Name="ttermpro" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ttermpro - Win32 Debug
!MESSAGE NMAKE /f "ttermpro.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "ttermpro.mak" CFG="ttermpro - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ttermpro - Win32 Release" ("Win32 (x86) Application")
!MESSAGE "ttermpro - Win32 Debug" ("Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ttermpro - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "bin\Release"
# PROP Intermediate_Dir "ttermpro\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\source\common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX"teraterm.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\source\common" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "bin\Debug"
# PROP Intermediate_Dir "ttermpro\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\source\common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX"teraterm.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /i "..\source\common" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ttermpro - Win32 Release"
# Name "ttermpro - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Source\Teraterm\filesys.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Teraterm\ftdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Teraterm\prnabort.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Teraterm\protodlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\stdafx.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Teraterm\tekwin.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Teraterm\teraprn.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Teraterm\teraterm.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Teraterm\vtwin.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\Source\Common\tek.ico
# End Source File
# Begin Source File

SOURCE=..\Source\Common\Teraterm.ico
# End Source File
# Begin Source File

SOURCE=..\Source\Teraterm\ttermpro.rc

!IF  "$(CFG)" == "ttermpro - Win32 Release"

!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Common\vt.ico
# End Source File
# End Group
# Begin Group "Source Files (C)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Source\Teraterm\buffer.c
# End Source File
# Begin Source File

SOURCE=..\Source\Teraterm\clipboar.c
# End Source File
# Begin Source File

SOURCE=..\Source\Teraterm\commlib.c
# End Source File
# Begin Source File

SOURCE=..\Source\Teraterm\keyboard.c
# End Source File
# Begin Source File

SOURCE=..\source\teraterm\teklib.c
# End Source File
# Begin Source File

SOURCE=..\Source\Teraterm\telnet.c
# End Source File
# Begin Source File

SOURCE=..\Source\Teraterm\ttdde.c
# End Source File
# Begin Source File

SOURCE=..\source\teraterm\ttdialog.c
# End Source File
# Begin Source File

SOURCE=..\Source\Teraterm\ttime.c
# End Source File
# Begin Source File

SOURCE=..\Source\Common\ttlib.c
# End Source File
# Begin Source File

SOURCE=..\source\teraterm\ttplug.c
# End Source File
# Begin Source File

SOURCE=..\source\teraterm\ttsetup.c
# End Source File
# Begin Source File

SOURCE=..\Source\Teraterm\ttwinman.c
# End Source File
# Begin Source File

SOURCE=..\Source\Teraterm\ttwsk.c
# End Source File
# Begin Source File

SOURCE=..\Source\Teraterm\vtdisp.c
# End Source File
# Begin Source File

SOURCE=..\Source\Teraterm\vtterm.c
# End Source File
# End Group
# Begin Group "Lib Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bin\Release\ttpcmn.lib
# End Source File
# End Group
# End Target
# End Project
