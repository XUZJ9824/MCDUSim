@echo off
rem dir mcduDlgRes*.* | find "mcduDlgRes"
rem
rem ---- check for wxrc.exe: should be in wxrc-util\vc_mswu or wxrc-util\vc_mswud
rem
cd wxrc-util
set WXRC_EXE_DIR=vc_mswu
if exist %WXRC_EXE_DIR%\wxrc.exe goto found_it
set WXRC_EXE_DIR=vc_mswud
if exist %WXRC_EXE_DIR%\wxrc.exe goto found_it
echo Unable to proceed -- wxrc-util\vc_msw*\wxrc.exe not found! Please build the project: wxrc
goto end
rem --------------------
rem
:found_it
rem
rem ---- check for mcduDlgRes.cpp
rem
if not exist ..\mcduDlgRes.cpp goto build_it
rem echo ### initial
dir ..\mcduDlgRes*.* | find "mcduDlgRes"
:build_it
rem
rem ---- build new mcduDlgRes.cpp in wxrc-util directory from mcduDlgs.xrc
rem
%WXRC_EXE_DIR%\wxrc.exe -c ..\mcduDlgs.xrc -o mcduDlgRes.cpp
cd ..
if exist wxrc-util\mcduDlgRes.cpp goto check
echo Unable to proceed -- wxrc-util\mcduDlgRes.cpp not created! Try using: cvs up mcduDlgRes.cpp
goto end
rem --------------------
rem
:check
rem
rem ---- check result: if new or changed, use it; if same, keep old version and delete new
rem
rem echo ### intermediate
rem dir mcduDlgRes*.* wxrc-util\mcduDlgRes*.* | find "mcduDlgRes"
if not exist mcduDlgRes.cpp goto use_new
fc /b mcduDlgRes.cpp wxrc-util\mcduDlgRes.cpp >nul: || goto changed
rem
rem
del wxrc-util\mcduDlgRes.cpp >nul:
goto done
rem --------------------
rem
:use_new
:changed
if exist mcduDlgRes.cpp attrib -r mcduDlgRes.cpp
if exist mcduDlgRes.cpp del mcduDlgRes.cpp >nul:
move wxrc-util\mcduDlgRes.cpp mcduDlgRes.cpp >nul:
rem
rem
:done
rem echo ### final
dir mcduDlgRes*.* | find "mcduDlgRes"
:end
@rem pause
@rem exit
