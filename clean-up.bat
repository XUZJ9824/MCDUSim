@echo off
del /F /Q mcduSim.vcproj.*.user >nul: 2>nul:
del /F /Q Debug\BuildLog.htm >nul: 2>nul:
del /F /Q Debug\mcduSim.exe.embed.manifest >nul: 2>nul:
del /F /Q Debug\mcduSim.exe.embed.manifest.res >nul: 2>nul:
del /F /Q Debug\mcduSim.exe.intermediate.manifest >nul: 2>nul:
del /F /Q Debug\mcduSim.map >nul: 2>nul:
del /F /Q Debug\mcdusim.res >nul: 2>nul:
del /F /Q Debug\mt.dep >nul: 2>nul:
del /F /Q Debug\vc80.idb >nul: 2>nul:
del /F /Q Installer\Release\McduSimInstaller.* >nul: 2>nul:
del /F /Q PythonMod\emstools.pyc >nul: 2>nul:
del /F /Q Release\BuildLog.htm >nul: 2>nul:
del /F /Q Release\exportedMcduScreen.jpg >nul: 2>nul:
del /F /Q Release\mcduSim.exe.intermediate.manifest >nul: 2>nul:
del /F /Q Release\mcduSim.map >nul: 2>nul:
del /F /Q Release\mcdusim.res >nul: 2>nul:
del /F /Q Release\mt.dep >nul: 2>nul:
del /F /Q Release\vc80.idb >nul: 2>nul:

del /F /Q TeraTermMod\visualc\ttermpro.vcproj.*.user >nul: 2>nul:
del /F /Q TeraTermMod\visualc\keycode\keycode.vcproj.7.10.old >nul: 2>nul:
del /F /Q TeraTermMod\visualc\keycode\keycode.vcproj.*.user >nul: 2>nul:
del /F /Q TeraTermMod\visualc\ttpcmn\ttpcmn.vcproj.7.10.old >nul: 2>nul:
del /F /Q TeraTermMod\visualc\ttpcmn\ttpcmn.vcproj.*.user >nul: 2>nul:
del /F /Q TeraTermMod\visualc\ttpdlg\ttpdlg.vcproj.7.10.old >nul: 2>nul:
del /F /Q TeraTermMod\visualc\ttpdlg\ttpdlg.vcproj.*.user >nul: 2>nul:
del /F /Q TeraTermMod\visualc\ttpfile\ttpfile.vcproj.7.10.old >nul: 2>nul:
del /F /Q TeraTermMod\visualc\ttpfile\ttpfile.vcproj.*.user >nul: 2>nul:
del /F /Q TeraTermMod\visualc\ttpmacro\ttpmacro.vcproj.7.10.old >nul: 2>nul:
del /F /Q TeraTermMod\visualc\ttpmacro\ttpmacro.vcproj.*.user >nul: 2>nul:
del /F /Q TeraTermMod\visualc\ttpset\ttpset.vcproj.7.10.old >nul: 2>nul:
del /F /Q TeraTermMod\visualc\ttpset\ttpset.vcproj.*.user >nul: 2>nul:
del /F /Q TeraTermMod\visualc\ttptek\ttptek.vcproj.7.10.old >nul: 2>nul:
del /F /Q TeraTermMod\visualc\ttptek\ttptek.vcproj.*.user >nul: 2>nul:

rmdir /S /Q Installer\Release >nul: 2>nul:
rmdir /S /Q TeraTermMod\visualc\bin >nul: 2>nul:
rmdir /S /Q TeraTermMod\visualc\ttermpro >nul: 2>nul:
rmdir /S /Q TeraTermMod\visualc\keycode\Debug >nul: 2>nul:
rmdir /S /Q TeraTermMod\visualc\keycode\Release >nul: 2>nul:
rmdir /S /Q TeraTermMod\visualc\ttpcmn\Debug >nul: 2>nul:
rmdir /S /Q TeraTermMod\visualc\ttpcmn\Release >nul: 2>nul:
rmdir /S /Q TeraTermMod\visualc\ttpdlg\Debug >nul: 2>nul:
rmdir /S /Q TeraTermMod\visualc\ttpdlg\Release >nul: 2>nul:
rmdir /S /Q TeraTermMod\visualc\ttpfile\Debug >nul: 2>nul:
rmdir /S /Q TeraTermMod\visualc\ttpfile\Release >nul: 2>nul:
rmdir /S /Q TeraTermMod\visualc\ttpmacro\Debug >nul: 2>nul:
rmdir /S /Q TeraTermMod\visualc\ttpmacro\Release >nul: 2>nul:
rmdir /S /Q TeraTermMod\visualc\ttpset\Debug >nul: 2>nul:
rmdir /S /Q TeraTermMod\visualc\ttpset\Release >nul: 2>nul:
rmdir /S /Q TeraTermMod\visualc\ttptek\Debug >nul: 2>nul:
rmdir /S /Q TeraTermMod\visualc\ttptek\Release >nul: 2>nul:
rmdir /S /Q mTermInstaller\Debug >nul: 2>nul:
rmdir /S /Q mTermInstaller\Release >nul: 2>nul:
rmdir /S /Q wxrc-util\vc_mswu >nul: 2>nul:
rmdir /S /Q wxrc-util\vc_mswud >nul: 2>nul: