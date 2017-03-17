# Microsoft Developer Studio Generated NMAKE File, Based on ttermpro.dsp
!IF "$(CFG)" == ""
CFG=ttermpro - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to keycode - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ttermpro - Win32 Release" && "$(CFG)" !=\
 "ttermpro - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "ttermpro.mak" CFG="ttermpro - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ttermpro - Win32 Release" ("Win32 (x86) Application" —p)
!MESSAGE "ttermpro - Win32 Debug" ("Win32 (x86) Application" —p)
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ttermpro - Win32 Release"

OUTDIR=.\bin\Release
INTDIR=.\ttermpro\Release
# Begin Custom Macros
OutDir=.\bin\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\ttermpro.exe"

!ELSE 

ALL : "ttpcmn - Win32 Release" "$(OUTDIR)\ttermpro.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"ttpcmn - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\buffer.obj"
	-@erase "$(INTDIR)\clipboar.obj"
	-@erase "$(INTDIR)\commlib.obj"
	-@erase "$(INTDIR)\Filesys.obj"
	-@erase "$(INTDIR)\ftdlg.obj"
	-@erase "$(INTDIR)\keyboard.obj"
	-@erase "$(INTDIR)\Prnabort.obj"
	-@erase "$(INTDIR)\protodlg.obj"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\Teklib.obj"
	-@erase "$(INTDIR)\tekwin.obj"
	-@erase "$(INTDIR)\telnet.obj"
	-@erase "$(INTDIR)\teraprn.obj"
	-@erase "$(INTDIR)\teraterm.obj"
	-@erase "$(INTDIR)\ttdde.obj"
	-@erase "$(INTDIR)\Ttdialog.obj"
	-@erase "$(INTDIR)\ttermpro.res"
	-@erase "$(INTDIR)\ttime.obj"
	-@erase "$(INTDIR)\ttlib.obj"
	-@erase "$(INTDIR)\ttsetup.obj"
	-@erase "$(INTDIR)\ttwinman.obj"
	-@erase "$(INTDIR)\ttwsk.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vtdisp.obj"
	-@erase "$(INTDIR)\vtterm.obj"
	-@erase "$(INTDIR)\vtwin.obj"
	-@erase "$(OUTDIR)\ttermpro.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\source\common" /D "WIN32" /D "NDEBUG"\
 /D "_WINDOWS" /Fp"$(INTDIR)\ttermpro.pch" /YX"teraterm.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\ttermpro\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ttermpro.res" /i "..\source\common" /d "NDEBUG"\
 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ttermpro.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\ttermpro.pdb" /machine:I386 /out:"$(OUTDIR)\ttermpro.exe" 
LINK32_OBJS= \
	"$(INTDIR)\buffer.obj" \
	"$(INTDIR)\clipboar.obj" \
	"$(INTDIR)\commlib.obj" \
	"$(INTDIR)\Filesys.obj" \
	"$(INTDIR)\ftdlg.obj" \
	"$(INTDIR)\keyboard.obj" \
	"$(INTDIR)\Prnabort.obj" \
	"$(INTDIR)\protodlg.obj" \
	"$(INTDIR)\stdafx.obj" \
	"$(INTDIR)\Teklib.obj" \
	"$(INTDIR)\tekwin.obj" \
	"$(INTDIR)\telnet.obj" \
	"$(INTDIR)\teraprn.obj" \
	"$(INTDIR)\teraterm.obj" \
	"$(INTDIR)\ttdde.obj" \
	"$(INTDIR)\Ttdialog.obj" \
	"$(INTDIR)\ttermpro.res" \
	"$(INTDIR)\ttime.obj" \
	"$(INTDIR)\ttlib.obj" \
	"$(INTDIR)\ttsetup.obj" \
	"$(INTDIR)\ttwinman.obj" \
	"$(INTDIR)\ttwsk.obj" \
	"$(INTDIR)\vtdisp.obj" \
	"$(INTDIR)\vtterm.obj" \
	"$(INTDIR)\vtwin.obj" \
	"$(OUTDIR)\ttpcmn.lib"

"$(OUTDIR)\ttermpro.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

OUTDIR=.\bin\Debug
INTDIR=.\ttermpro\Debug
# Begin Custom Macros
OutDir=.\bin\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\ttermpro.exe"

!ELSE 

ALL : "ttpcmn - Win32 Debug" "$(OUTDIR)\ttermpro.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"ttpcmn - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\buffer.obj"
	-@erase "$(INTDIR)\clipboar.obj"
	-@erase "$(INTDIR)\commlib.obj"
	-@erase "$(INTDIR)\Filesys.obj"
	-@erase "$(INTDIR)\ftdlg.obj"
	-@erase "$(INTDIR)\keyboard.obj"
	-@erase "$(INTDIR)\Prnabort.obj"
	-@erase "$(INTDIR)\protodlg.obj"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\Teklib.obj"
	-@erase "$(INTDIR)\tekwin.obj"
	-@erase "$(INTDIR)\telnet.obj"
	-@erase "$(INTDIR)\teraprn.obj"
	-@erase "$(INTDIR)\teraterm.obj"
	-@erase "$(INTDIR)\ttdde.obj"
	-@erase "$(INTDIR)\Ttdialog.obj"
	-@erase "$(INTDIR)\ttermpro.res"
	-@erase "$(INTDIR)\ttime.obj"
	-@erase "$(INTDIR)\ttlib.obj"
	-@erase "$(INTDIR)\ttsetup.obj"
	-@erase "$(INTDIR)\ttwinman.obj"
	-@erase "$(INTDIR)\ttwsk.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\vtdisp.obj"
	-@erase "$(INTDIR)\vtterm.obj"
	-@erase "$(INTDIR)\vtwin.obj"
	-@erase "$(OUTDIR)\ttermpro.exe"
	-@erase "$(OUTDIR)\ttermpro.ilk"
	-@erase "$(OUTDIR)\ttermpro.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\source\common" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\ttermpro.pch" /YX"teraterm.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\ttermpro\Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x411 /fo"$(INTDIR)\ttermpro.res" /i "..\source\common" /d "_DEBUG"\
 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ttermpro.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)\ttermpro.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\ttermpro.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\buffer.obj" \
	"$(INTDIR)\clipboar.obj" \
	"$(INTDIR)\commlib.obj" \
	"$(INTDIR)\Filesys.obj" \
	"$(INTDIR)\ftdlg.obj" \
	"$(INTDIR)\keyboard.obj" \
	"$(INTDIR)\Prnabort.obj" \
	"$(INTDIR)\protodlg.obj" \
	"$(INTDIR)\stdafx.obj" \
	"$(INTDIR)\Teklib.obj" \
	"$(INTDIR)\tekwin.obj" \
	"$(INTDIR)\telnet.obj" \
	"$(INTDIR)\teraprn.obj" \
	"$(INTDIR)\teraterm.obj" \
	"$(INTDIR)\ttdde.obj" \
	"$(INTDIR)\Ttdialog.obj" \
	"$(INTDIR)\ttermpro.res" \
	"$(INTDIR)\ttime.obj" \
	"$(INTDIR)\ttlib.obj" \
	"$(INTDIR)\ttsetup.obj" \
	"$(INTDIR)\ttwinman.obj" \
	"$(INTDIR)\ttwsk.obj" \
	"$(INTDIR)\vtdisp.obj" \
	"$(INTDIR)\vtterm.obj" \
	"$(INTDIR)\vtwin.obj" \
	"$(OUTDIR)\ttpcmn.lib" \
	".\bin\Release\ttpcmn.lib"

"$(OUTDIR)\ttermpro.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(CFG)" == "ttermpro - Win32 Release" || "$(CFG)" ==\
 "ttermpro - Win32 Debug"
SOURCE=..\Source\Teraterm\buffer.c

!IF  "$(CFG)" == "ttermpro - Win32 Release"

DEP_CPP_BUFFE=\
	"..\source\common\teraterm.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\buffer.h"\
	"..\source\teraterm\clipboar.h"\
	"..\source\teraterm\telnet.h"\
	"..\source\teraterm\teraprn.h"\
	"..\source\teraterm\ttwinman.h"\
	"..\source\teraterm\vtdisp.h"\
	

"$(INTDIR)\buffer.obj" : $(SOURCE) $(DEP_CPP_BUFFE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

DEP_CPP_BUFFE=\
	"..\source\common\teraterm.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\buffer.h"\
	"..\source\teraterm\clipboar.h"\
	"..\source\teraterm\telnet.h"\
	"..\source\teraterm\teraprn.h"\
	"..\source\teraterm\ttwinman.h"\
	"..\source\teraterm\vtdisp.h"\
	

"$(INTDIR)\buffer.obj" : $(SOURCE) $(DEP_CPP_BUFFE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Source\Teraterm\clipboar.c

!IF  "$(CFG)" == "ttermpro - Win32 Release"

DEP_CPP_CLIPB=\
	"..\source\common\teraterm.h"\
	"..\source\common\ttcommon.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\clipboar.h"\
	"..\source\teraterm\ttwinman.h"\
	

"$(INTDIR)\clipboar.obj" : $(SOURCE) $(DEP_CPP_CLIPB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

DEP_CPP_CLIPB=\
	"..\source\common\teraterm.h"\
	"..\source\common\ttcommon.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\clipboar.h"\
	"..\source\teraterm\ttwinman.h"\
	

"$(INTDIR)\clipboar.obj" : $(SOURCE) $(DEP_CPP_CLIPB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Source\Teraterm\commlib.c

!IF  "$(CFG)" == "ttermpro - Win32 Release"

DEP_CPP_COMML=\
	"..\source\common\teraterm.h"\
	"..\source\common\ttcommon.h"\
	"..\source\common\ttlib.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\commlib.h"\
	"..\source\teraterm\ttwsk.h"\
	

"$(INTDIR)\commlib.obj" : $(SOURCE) $(DEP_CPP_COMML) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

DEP_CPP_COMML=\
	"..\source\common\teraterm.h"\
	"..\source\common\ttcommon.h"\
	"..\source\common\ttlib.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\commlib.h"\
	"..\source\teraterm\ttwsk.h"\
	

"$(INTDIR)\commlib.obj" : $(SOURCE) $(DEP_CPP_COMML) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Source\Teraterm\Filesys.cpp

!IF  "$(CFG)" == "ttermpro - Win32 Release"

DEP_CPP_FILES=\
	"..\source\common\helpid.h"\
	"..\source\common\stdafx.h"\
	"..\source\common\teraterm.h"\
	"..\source\common\ttcommon.h"\
	"..\source\common\ttftypes.h"\
	"..\source\common\ttlib.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\commlib.h"\
	"..\source\teraterm\filesys.h"\
	"..\source\teraterm\ftdlg.h"\
	"..\source\teraterm\protodlg.h"\
	"..\source\teraterm\ttdde.h"\
	"..\source\teraterm\ttwinman.h"\
	

"$(INTDIR)\Filesys.obj" : $(SOURCE) $(DEP_CPP_FILES) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

DEP_CPP_FILES=\
	"..\source\common\helpid.h"\
	"..\source\common\stdafx.h"\
	"..\source\common\teraterm.h"\
	"..\source\common\ttcommon.h"\
	"..\source\common\ttftypes.h"\
	"..\source\common\ttlib.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\commlib.h"\
	"..\source\teraterm\filesys.h"\
	"..\source\teraterm\ftdlg.h"\
	"..\source\teraterm\protodlg.h"\
	"..\source\teraterm\ttdde.h"\
	"..\source\teraterm\ttwinman.h"\
	

"$(INTDIR)\Filesys.obj" : $(SOURCE) $(DEP_CPP_FILES) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Source\Teraterm\ftdlg.cpp

!IF  "$(CFG)" == "ttermpro - Win32 Release"

DEP_CPP_FTDLG=\
	"..\source\common\stdafx.h"\
	"..\source\common\teraterm.h"\
	"..\source\common\ttftypes.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\ftdlg.h"\
	

"$(INTDIR)\ftdlg.obj" : $(SOURCE) $(DEP_CPP_FTDLG) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

DEP_CPP_FTDLG=\
	"..\source\common\stdafx.h"\
	"..\source\common\teraterm.h"\
	"..\source\common\ttftypes.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\ftdlg.h"\
	

"$(INTDIR)\ftdlg.obj" : $(SOURCE) $(DEP_CPP_FTDLG) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Source\Teraterm\keyboard.c

!IF  "$(CFG)" == "ttermpro - Win32 Release"

DEP_CPP_KEYBO=\
	"..\source\common\teraterm.h"\
	"..\source\common\ttcommon.h"\
	"..\source\common\ttlib.h"\
	"..\source\common\ttsetup.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\keyboard.h"\
	"..\source\teraterm\ttdde.h"\
	"..\source\teraterm\ttwinman.h"\
	

"$(INTDIR)\keyboard.obj" : $(SOURCE) $(DEP_CPP_KEYBO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

DEP_CPP_KEYBO=\
	"..\source\common\teraterm.h"\
	"..\source\common\ttcommon.h"\
	"..\source\common\ttlib.h"\
	"..\source\common\ttsetup.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\keyboard.h"\
	"..\source\teraterm\ttdde.h"\
	"..\source\teraterm\ttwinman.h"\
	

"$(INTDIR)\keyboard.obj" : $(SOURCE) $(DEP_CPP_KEYBO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Source\Teraterm\Prnabort.cpp

!IF  "$(CFG)" == "ttermpro - Win32 Release"

DEP_CPP_PRNAB=\
	"..\source\common\stdafx.h"\
	"..\source\common\teraterm.h"\
	"..\source\teraterm\prnabort.h"\
	

"$(INTDIR)\Prnabort.obj" : $(SOURCE) $(DEP_CPP_PRNAB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

DEP_CPP_PRNAB=\
	"..\source\common\stdafx.h"\
	"..\source\common\teraterm.h"\
	"..\source\teraterm\prnabort.h"\
	

"$(INTDIR)\Prnabort.obj" : $(SOURCE) $(DEP_CPP_PRNAB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Source\Teraterm\protodlg.cpp

!IF  "$(CFG)" == "ttermpro - Win32 Release"

DEP_CPP_PROTO=\
	"..\source\common\stdafx.h"\
	"..\source\common\teraterm.h"\
	"..\source\common\ttftypes.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\protodlg.h"\
	

"$(INTDIR)\protodlg.obj" : $(SOURCE) $(DEP_CPP_PROTO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

DEP_CPP_PROTO=\
	"..\source\common\stdafx.h"\
	"..\source\common\teraterm.h"\
	"..\source\common\ttftypes.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\protodlg.h"\
	

"$(INTDIR)\protodlg.obj" : $(SOURCE) $(DEP_CPP_PROTO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Source\Common\stdafx.cpp
DEP_CPP_STDAF=\
	"..\source\common\stdafx.h"\
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Source\Teraterm\Teklib.c

!IF  "$(CFG)" == "ttermpro - Win32 Release"

DEP_CPP_TEKLI=\
	"..\source\common\tektypes.h"\
	"..\source\common\teraterm.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\teklib.h"\
	

"$(INTDIR)\Teklib.obj" : $(SOURCE) $(DEP_CPP_TEKLI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

DEP_CPP_TEKLI=\
	"..\source\common\tektypes.h"\
	"..\source\common\teraterm.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\teklib.h"\
	

"$(INTDIR)\Teklib.obj" : $(SOURCE) $(DEP_CPP_TEKLI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Source\Teraterm\tekwin.cpp

!IF  "$(CFG)" == "ttermpro - Win32 Release"

DEP_CPP_TEKWI=\
	"..\source\common\helpid.h"\
	"..\source\common\stdafx.h"\
	"..\source\common\tektypes.h"\
	"..\source\common\teraterm.h"\
	"..\source\common\ttcommon.h"\
	"..\source\common\ttdialog.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\clipboar.h"\
	"..\source\teraterm\keyboard.h"\
	"..\source\teraterm\teklib.h"\
	"..\source\teraterm\tekwin.h"\
	"..\source\teraterm\teraprn.h"\
	"..\source\teraterm\ttwinman.h"\
	

"$(INTDIR)\tekwin.obj" : $(SOURCE) $(DEP_CPP_TEKWI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

DEP_CPP_TEKWI=\
	"..\source\common\helpid.h"\
	"..\source\common\stdafx.h"\
	"..\source\common\tektypes.h"\
	"..\source\common\teraterm.h"\
	"..\source\common\ttcommon.h"\
	"..\source\common\ttdialog.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\clipboar.h"\
	"..\source\teraterm\keyboard.h"\
	"..\source\teraterm\teklib.h"\
	"..\source\teraterm\tekwin.h"\
	"..\source\teraterm\teraprn.h"\
	"..\source\teraterm\ttwinman.h"\
	

"$(INTDIR)\tekwin.obj" : $(SOURCE) $(DEP_CPP_TEKWI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Source\Teraterm\telnet.c

!IF  "$(CFG)" == "ttermpro - Win32 Release"

DEP_CPP_TELNE=\
	"..\source\common\teraterm.h"\
	"..\source\common\ttcommon.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\commlib.h"\
	"..\source\teraterm\telnet.h"\
	"..\source\teraterm\ttwinman.h"\
	

"$(INTDIR)\telnet.obj" : $(SOURCE) $(DEP_CPP_TELNE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

DEP_CPP_TELNE=\
	"..\source\common\teraterm.h"\
	"..\source\common\ttcommon.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\commlib.h"\
	"..\source\teraterm\telnet.h"\
	"..\source\teraterm\ttwinman.h"\
	

"$(INTDIR)\telnet.obj" : $(SOURCE) $(DEP_CPP_TELNE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Source\Teraterm\teraprn.cpp

!IF  "$(CFG)" == "ttermpro - Win32 Release"

DEP_CPP_TERAP=\
	"..\source\common\stdafx.h"\
	"..\source\common\teraterm.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\prnabort.h"\
	"..\source\teraterm\teraprn.h"\
	"..\source\teraterm\ttwinman.h"\
	

"$(INTDIR)\teraprn.obj" : $(SOURCE) $(DEP_CPP_TERAP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

DEP_CPP_TERAP=\
	"..\source\common\stdafx.h"\
	"..\source\common\teraterm.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\prnabort.h"\
	"..\source\teraterm\teraprn.h"\
	"..\source\teraterm\ttwinman.h"\
	

"$(INTDIR)\teraprn.obj" : $(SOURCE) $(DEP_CPP_TERAP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Source\Teraterm\teraterm.cpp

!IF  "$(CFG)" == "ttermpro - Win32 Release"

DEP_CPP_TERAT=\
	"..\source\common\stdafx.h"\
	"..\source\common\tektypes.h"\
	"..\source\common\teraterm.h"\
	"..\source\common\ttftypes.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\buffer.h"\
	"..\source\teraterm\clipboar.h"\
	"..\source\teraterm\commlib.h"\
	"..\source\teraterm\filesys.h"\
	"..\source\teraterm\tekwin.h"\
	"..\source\teraterm\telnet.h"\
	"..\source\teraterm\teraapp.h"\
	"..\source\teraterm\ttdde.h"\
	"..\source\teraterm\ttwinman.h"\
	"..\source\teraterm\vtterm.h"\
	"..\source\teraterm\vtwin.h"\
	

"$(INTDIR)\teraterm.obj" : $(SOURCE) $(DEP_CPP_TERAT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

DEP_CPP_TERAT=\
	"..\source\common\stdafx.h"\
	"..\source\common\tektypes.h"\
	"..\source\common\teraterm.h"\
	"..\source\common\ttftypes.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\buffer.h"\
	"..\source\teraterm\clipboar.h"\
	"..\source\teraterm\commlib.h"\
	"..\source\teraterm\filesys.h"\
	"..\source\teraterm\tekwin.h"\
	"..\source\teraterm\telnet.h"\
	"..\source\teraterm\teraapp.h"\
	"..\source\teraterm\ttdde.h"\
	"..\source\teraterm\ttwinman.h"\
	"..\source\teraterm\vtterm.h"\
	"..\source\teraterm\vtwin.h"\
	

"$(INTDIR)\teraterm.obj" : $(SOURCE) $(DEP_CPP_TERAT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Source\Teraterm\ttdde.c

!IF  "$(CFG)" == "ttermpro - Win32 Release"

DEP_CPP_TTDDE=\
	"..\source\common\teraterm.h"\
	"..\source\common\ttftypes.h"\
	"..\source\common\ttlib.h"\
	"..\source\common\ttsetup.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\clipboar.h"\
	"..\source\teraterm\filesys.h"\
	"..\source\teraterm\telnet.h"\
	"..\source\teraterm\ttdde.h"\
	"..\source\teraterm\ttwinman.h"\
	

"$(INTDIR)\ttdde.obj" : $(SOURCE) $(DEP_CPP_TTDDE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

DEP_CPP_TTDDE=\
	"..\source\common\teraterm.h"\
	"..\source\common\ttftypes.h"\
	"..\source\common\ttlib.h"\
	"..\source\common\ttsetup.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\clipboar.h"\
	"..\source\teraterm\filesys.h"\
	"..\source\teraterm\telnet.h"\
	"..\source\teraterm\ttdde.h"\
	"..\source\teraterm\ttwinman.h"\
	

"$(INTDIR)\ttdde.obj" : $(SOURCE) $(DEP_CPP_TTDDE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Source\Common\Ttdialog.c

!IF  "$(CFG)" == "ttermpro - Win32 Release"

DEP_CPP_TTDIA=\
	"..\source\common\teraterm.h"\
	"..\source\common\ttdialog.h"\
	"..\source\common\tttypes.h"\
	

"$(INTDIR)\Ttdialog.obj" : $(SOURCE) $(DEP_CPP_TTDIA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

DEP_CPP_TTDIA=\
	"..\source\common\teraterm.h"\
	"..\source\common\ttdialog.h"\
	"..\source\common\tttypes.h"\
	

"$(INTDIR)\Ttdialog.obj" : $(SOURCE) $(DEP_CPP_TTDIA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Source\Teraterm\ttermpro.rc
DEP_RSC_TTERM=\
	"..\Source\Common\Tek.ico"\
	"..\Source\Common\Teraterm.ico"\
	"..\Source\Common\Vt.ico"\
	

!IF  "$(CFG)" == "ttermpro - Win32 Release"


"$(INTDIR)\ttermpro.res" : $(SOURCE) $(DEP_RSC_TTERM) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\ttermpro.res" /i "..\source\common" /i\
 "\Dev\Teraterm\Source\Teraterm" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"


"$(INTDIR)\ttermpro.res" : $(SOURCE) $(DEP_RSC_TTERM) "$(INTDIR)"
	$(RSC) /l 0x411 /fo"$(INTDIR)\ttermpro.res" /i "..\source\common" /i\
 "\Dev\Teraterm\Source\Teraterm" /d "_DEBUG" $(SOURCE)


!ENDIF 

SOURCE=..\Source\Teraterm\ttime.c

!IF  "$(CFG)" == "ttermpro - Win32 Release"

DEP_CPP_TTIME=\
	"..\source\common\teraterm.h"\
	"..\source\common\ttcommon.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\ttime.h"\
	"..\source\teraterm\ttwinman.h"\
	

"$(INTDIR)\ttime.obj" : $(SOURCE) $(DEP_CPP_TTIME) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

DEP_CPP_TTIME=\
	"..\source\common\teraterm.h"\
	"..\source\common\ttcommon.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\ttime.h"\
	"..\source\teraterm\ttwinman.h"\
	

"$(INTDIR)\ttime.obj" : $(SOURCE) $(DEP_CPP_TTIME) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Source\Common\ttlib.c
DEP_CPP_TTLIB=\
	"..\source\common\teraterm.h"\
	

"$(INTDIR)\ttlib.obj" : $(SOURCE) $(DEP_CPP_TTLIB) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Source\Common\ttsetup.c

!IF  "$(CFG)" == "ttermpro - Win32 Release"

DEP_CPP_TTSET=\
	"..\source\common\teraterm.h"\
	"..\source\common\ttsetup.h"\
	"..\source\common\tttypes.h"\
	

"$(INTDIR)\ttsetup.obj" : $(SOURCE) $(DEP_CPP_TTSET) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

DEP_CPP_TTSET=\
	"..\source\common\teraterm.h"\
	"..\source\common\ttsetup.h"\
	"..\source\common\tttypes.h"\
	

"$(INTDIR)\ttsetup.obj" : $(SOURCE) $(DEP_CPP_TTSET) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Source\Teraterm\ttwinman.c

!IF  "$(CFG)" == "ttermpro - Win32 Release"

DEP_CPP_TTWIN=\
	"..\source\common\helpid.h"\
	"..\source\common\teraterm.h"\
	"..\source\common\ttlib.h"\
	"..\source\common\tttypes.h"\
	

"$(INTDIR)\ttwinman.obj" : $(SOURCE) $(DEP_CPP_TTWIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

DEP_CPP_TTWIN=\
	"..\source\common\helpid.h"\
	"..\source\common\teraterm.h"\
	"..\source\common\ttlib.h"\
	"..\source\common\tttypes.h"\
	

"$(INTDIR)\ttwinman.obj" : $(SOURCE) $(DEP_CPP_TTWIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Source\Teraterm\ttwsk.c

!IF  "$(CFG)" == "ttermpro - Win32 Release"

DEP_CPP_TTWSK=\
	"..\source\common\teraterm.h"\
	"..\source\teraterm\ttwsk.h"\
	

"$(INTDIR)\ttwsk.obj" : $(SOURCE) $(DEP_CPP_TTWSK) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

DEP_CPP_TTWSK=\
	"..\source\common\teraterm.h"\
	"..\source\teraterm\ttwsk.h"\
	

"$(INTDIR)\ttwsk.obj" : $(SOURCE) $(DEP_CPP_TTWSK) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Source\Teraterm\vtdisp.c

!IF  "$(CFG)" == "ttermpro - Win32 Release"

DEP_CPP_VTDIS=\
	"..\source\common\teraterm.h"\
	"..\source\common\ttcommon.h"\
	"..\source\common\ttdialog.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\ttime.h"\
	"..\source\teraterm\ttwinman.h"\
	"..\source\teraterm\vtdisp.h"\
	

"$(INTDIR)\vtdisp.obj" : $(SOURCE) $(DEP_CPP_VTDIS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

DEP_CPP_VTDIS=\
	"..\source\common\teraterm.h"\
	"..\source\common\ttcommon.h"\
	"..\source\common\ttdialog.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\ttime.h"\
	"..\source\teraterm\ttwinman.h"\
	"..\source\teraterm\vtdisp.h"\
	

"$(INTDIR)\vtdisp.obj" : $(SOURCE) $(DEP_CPP_VTDIS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Source\Teraterm\vtterm.c

!IF  "$(CFG)" == "ttermpro - Win32 Release"

DEP_CPP_VTTER=\
	"..\source\common\teraterm.h"\
	"..\source\common\ttcommon.h"\
	"..\source\common\ttftypes.h"\
	"..\source\common\ttlib.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\buffer.h"\
	"..\source\teraterm\commlib.h"\
	"..\source\teraterm\filesys.h"\
	"..\source\teraterm\keyboard.h"\
	"..\source\teraterm\telnet.h"\
	"..\source\teraterm\teraprn.h"\
	"..\source\teraterm\ttwinman.h"\
	"..\source\teraterm\vtdisp.h"\
	"..\source\teraterm\vtterm.h"\
	

"$(INTDIR)\vtterm.obj" : $(SOURCE) $(DEP_CPP_VTTER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

DEP_CPP_VTTER=\
	"..\source\common\teraterm.h"\
	"..\source\common\ttcommon.h"\
	"..\source\common\ttftypes.h"\
	"..\source\common\ttlib.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\buffer.h"\
	"..\source\teraterm\commlib.h"\
	"..\source\teraterm\filesys.h"\
	"..\source\teraterm\keyboard.h"\
	"..\source\teraterm\telnet.h"\
	"..\source\teraterm\teraprn.h"\
	"..\source\teraterm\ttwinman.h"\
	"..\source\teraterm\vtdisp.h"\
	"..\source\teraterm\vtterm.h"\
	

"$(INTDIR)\vtterm.obj" : $(SOURCE) $(DEP_CPP_VTTER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\Source\Teraterm\vtwin.cpp

!IF  "$(CFG)" == "ttermpro - Win32 Release"

DEP_CPP_VTWIN=\
	"..\source\common\helpid.h"\
	"..\source\common\stdafx.h"\
	"..\source\common\tektypes.h"\
	"..\source\common\teraterm.h"\
	"..\source\common\ttcommon.h"\
	"..\source\common\ttdialog.h"\
	"..\source\common\ttftypes.h"\
	"..\source\common\ttlib.h"\
	"..\source\common\ttsetup.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\buffer.h"\
	"..\source\teraterm\clipboar.h"\
	"..\source\teraterm\commlib.h"\
	"..\source\teraterm\filesys.h"\
	"..\source\teraterm\keyboard.h"\
	"..\source\teraterm\tekwin.h"\
	"..\source\teraterm\telnet.h"\
	"..\source\teraterm\ttdde.h"\
	"..\source\teraterm\ttime.h"\
	"..\source\teraterm\ttwinman.h"\
	"..\source\teraterm\vtdisp.h"\
	"..\source\teraterm\vtterm.h"\
	"..\source\teraterm\vtwin.h"\
	

"$(INTDIR)\vtwin.obj" : $(SOURCE) $(DEP_CPP_VTWIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

DEP_CPP_VTWIN=\
	"..\source\common\helpid.h"\
	"..\source\common\stdafx.h"\
	"..\source\common\tektypes.h"\
	"..\source\common\teraterm.h"\
	"..\source\common\ttcommon.h"\
	"..\source\common\ttdialog.h"\
	"..\source\common\ttftypes.h"\
	"..\source\common\ttlib.h"\
	"..\source\common\ttsetup.h"\
	"..\source\common\tttypes.h"\
	"..\source\teraterm\buffer.h"\
	"..\source\teraterm\clipboar.h"\
	"..\source\teraterm\commlib.h"\
	"..\source\teraterm\filesys.h"\
	"..\source\teraterm\keyboard.h"\
	"..\source\teraterm\tekwin.h"\
	"..\source\teraterm\telnet.h"\
	"..\source\teraterm\ttdde.h"\
	"..\source\teraterm\ttime.h"\
	"..\source\teraterm\ttwinman.h"\
	"..\source\teraterm\vtdisp.h"\
	"..\source\teraterm\vtterm.h"\
	"..\source\teraterm\vtwin.h"\
	

"$(INTDIR)\vtwin.obj" : $(SOURCE) $(DEP_CPP_VTWIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

!IF  "$(CFG)" == "ttermpro - Win32 Release"

"ttpcmn - Win32 Release" : 
   cd ".\ttpcmn"
   $(MAKE) /$(MAKEFLAGS) /F .\ttpcmn.mak CFG="ttpcmn - Win32 Release" 
   cd ".."

"ttpcmn - Win32 ReleaseCLEAN" : 
   cd ".\ttpcmn"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\ttpcmn.mak CFG="ttpcmn - Win32 Release"\
 RECURSE=1 
   cd ".."

!ELSEIF  "$(CFG)" == "ttermpro - Win32 Debug"

"ttpcmn - Win32 Debug" : 
   cd ".\ttpcmn"
   $(MAKE) /$(MAKEFLAGS) /F .\ttpcmn.mak CFG="ttpcmn - Win32 Debug" 
   cd ".."

"ttpcmn - Win32 DebugCLEAN" : 
   cd ".\ttpcmn"
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\ttpcmn.mak CFG="ttpcmn - Win32 Debug"\
 RECURSE=1 
   cd ".."

!ENDIF 


!ENDIF 

