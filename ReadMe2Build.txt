

For Users:
=========


   Installing MCDU and CMC Simulators
   ==================================

- Unzip the installer locate at:

\\Sentry\Boeing-777\Software\Tools\McduSim_Intaller\McduSimInstaller_v102.zip (or latest version)

- double-click on the extracted McduSimInstaller.msi file (and install it as any Windows app).

- The installer will create shortcuts on Desktop and Start menus under "EMS Satcom Tools".


For developers only:
====================


    How to build MCDU Simulator
    ===========================

- Install MinGW toolkit.

- Add to the path the <Where_you_install_MinGW>\bin;

tip: use the "System Properties" -> "Environment Variable" to add these directory permanently to the system path.

- Checkout the source code from SVN location:

    http://devstar/Hub/Darth/Aero/trunk/Utilities/Generic/MCDU_CMC_Simulators

- open a cmd windows and goto <your_MCDU_CMC_Simulators_dir>\trunk\mcdu directory

- run the makesim.bat file (just type makesim in a cmd window).


    How to build CMC Simulator
    ===========================

- use the same tools setup than the above (MinGW, directories, etc);

- goto <your_MCDU_CMC_Simulators_dir>\trunk\cmc directory

- run the makesim.bat file (just type makesim in the cmd window).


   Running the Simulators without using the Installer
   ==================================================

- Before running you need to add Dynamic library dependencies to the path:

    <your_MCDU_CMC_Simulators_dir>\trunk\mcdu\wxWidgets\lib\gcc_dll;
    and
    <your_MCDU_CMC_Simulators_dir>\trunk\mcdu\DDC\Lib

You can use either the "System Properties" -> "Environment Variable" to add those directories permanently to path or simple type:

path=%path%;<your_MCDU_CMC_Simulators_dir>\trunk\mcdu\wxWidgets\lib\gcc_dll;<your_MCDU_CMC_Simulators_dir>\trunk\mcdu\DDC\Lib;
(it will just work for the current cmd window).


- To run the MCDU simulator: goto <your_MCDU_CMC_Simulators_dir>\trunk\mcdu directory in a cmd window and just type gcc_mswdll\mcdusim.exe

- To run the CMC simulator: goto <your_MCDU_CMC_Simulators_dir>\trunk\cmc directory in a cmd window and just type gcc_mswdll\cmcsim.exe


   Running the Simulators using the Installer
   ==========================================

- Just double-click the installer locate at:

<your_MCDU_CMC_Simulators_dir>\trunk\mcdu\Installer\Release\McduSimInstaller.msi

- The installer will create shortcuts on Desktop and Start menus under "EMS Satcom Tools".

    Installer Project
    =================

There is a Visual Studio installer project for MCDU and CMC simulators at <your_MCDU_CMC_Simulators_dir>\trunk\mcdu\Installer
It is good for VC 2008.


mTerm project
=============

The HSD710 can communicate with MCDU simulator using RS232. MCDU Sim must be select to work under UDP protocol (Connection UDP).
mTerm must be used as a VT100 Terminal connected to SDU's maintanace port.

mTerm Telnet:
=============

mTerm has another cool feature: Telnet clients can connect to mTerm port 9000. Therefore, tools like Python scripts, can use Telnet to "talk" with SDU emulating a terminal commands via mTerm.


  mTerm Installer:
  ================

<your_MCDU_CMC_Simulators_dir>\trunk\mcdu\mTermInstaller\Release\mTermInstaller.msi

  for users:
  =========

\\Sentry\Boeing-777\Software\Tools\McduSim_Intaller\mTermInstaller.zip

  mTerm source code:
  ==================

<your_MCDU_CMC_Simulators_dir>\trunk\mcdu\TeraTermMod
Was built with VC 2003.



