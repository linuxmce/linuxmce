@echo on

echo Deleting old pluto.makerelease folder...
rmdir "/pluto.makerelease" /S /Q

echo Checking out pluto sources...
cd "c:\Program Files\Subversion\bin"

svn co http://svn.linuxmce.com/pluto/trunk/src /pluto.makerelease/trunk/src
svn co http://svn.linuxmce.com/pluto/trunk/libs /pluto.makerelease/trunk/libs
svn co http://svn.linuxmce.com/pluto/trunk/installers/Installers /pluto.makerelease/trunk/installers/Installers

echo BUILDING PLUTO...
cd "c:\pluto.makerelease\trunk\src"
call "SwitchToPluto.bat"

echo Building pluto(symbian) sources...
cd "c:\pluto.makerelease\trunk\src"
call "MakeRelease_Symbian_S60.bat"

echo Building pluto(windows) sources...
cd "c:\pluto.makerelease\trunk\src"
call "MakeRelease_Windows.bat"

echo Building pluto(windows ce) sources...
cd "c:\pluto.makerelease\trunk\src"
call "MakeRelease_WindowsCE.bat"

cd "c:\pluto.makerelease\trunk\src\bin"
copy Orbiter.exe Orbiter_Win32.dat
copy Orbiter_CeNet4_XScale.exe Orbiter_CeNet4_XScale.dat
copy Orbiter_CeNet4_x86.exe Orbiter_CeNet4_x86.dat

echo COPYING PLUTO FILES...

echo Copying EXE files...
copy *.exe "\\10.0.2.4\builds\Windows_Output\src\bin"
echo Copying DLL files...
copy *.dll "\\10.0.2.4\builds\Windows_Output\src\bin"
echo Copying DAT files...
copy *.dat "\\10.0.2.4\builds\Windows_Output\src\bin"
echo Copying SIS files...
copy *.sis "\\10.0.2.4\builds\Windows_Output\src\bin"
echo Copying CAB files...
copy *.cab "\\10.0.2.4\builds\Windows_Output\src\bin"

echo Copying LIB files...
cd "c:\pluto.makerelease\trunk\src\lib"
copy *.lib "\\10.0.2.4\builds\Windows_Output\src\lib"

echo Copying MSI files...
cd C:\pluto.makerelease\trunk\installers\Installers\Orbiter\Release\
copy *.msi "\\10.0.2.4\builds\Windows_Output\src\bin"

echo Done. (pluto)

echo ==============================================================================================

echo BUILDING MONSTER...
cd "c:\pluto.makerelease\trunk\src"
call "SwitchToMonster.bat"

echo CE NET
cd "c:\pluto.makerelease\trunk\src\Orbiter\CENet"
call "build_cab.bat"
cd "c:\pluto.makerelease\trunk\src\Orbiter\CENet_PocketFrog"
call "build_cab.bat"
cd "c:\pluto.makerelease\trunk\src\Orbiter\Orbiter_CeNet4_x86"
call "build_cab.bat"

echo SMARTPHONE
cd "c:\pluto.makerelease\trunk\src\Orbiter\CENet_Smartphone"
call "build_cab.bat"

echo SMARTPHONE_TREO
cd "c:\pluto.makerelease\trunk\src\Orbiter\CENet_Smartphone_Treo"
call "build_cab.bat"

echo XP
cd C:\pluto.makerelease\trunk\installers\Installers\Orbiter
call WinMake.bat
cd C:\pluto.makerelease\trunk\installers\Installers\Orbiter\Release\
copy *.msi C:\pluto.makerelease\trunk\src\bin

echo COPYING MONSTER FILES...

cd "c:\pluto.makerelease\trunk\src\bin"


echo Copying EXE files...
copy *.exe "\\10.0.2.4\builds\Windows_Output_Monster\src\bin"
echo Copying DLL files...
copy *.dll "\\10.0.2.4\builds\Windows_Output_Monster\src\bin"
echo Copying DAT files...
copy *.dat "\\10.0.2.4\builds\Windows_Output_Monster\src\bin"
echo Copying SIS files...
copy *.sis "\\10.0.2.4\builds\Windows_Output_Monster\src\bin"
echo Copying CAB files...
copy *.cab "\\10.0.2.4\builds\Windows_Output_Monster\src\bin"

echo Copying LIB files...
cd "c:\pluto.makerelease\trunk\src\lib"
copy *.lib "\\10.0.2.4\builds\Windows_Output_Monster\src\lib"

echo Copying MSI files...
cd C:\pluto.makerelease\trunk\installers\Installers\Orbiter\Release\
copy *.msi "\\10.0.2.4\builds\Windows_Output_Monster\src\bin"

echo Done. (monster)

echo ==============================================================================================

echo BUILDING LINUXMCE...
cd "c:\pluto.makerelease\trunk\src"
call "SwitchToLinuxMCE.bat"

echo CE NET
cd "c:\pluto.makerelease\trunk\src\Orbiter\CENet"
call "build_cab.bat"
cd "c:\pluto.makerelease\trunk\src\Orbiter\CENet_PocketFrog"
call "build_cab.bat"
cd "c:\pluto.makerelease\trunk\src\Orbiter\Orbiter_CeNet4_x86"
call "build_cab.bat"

echo SMARTPHONE
cd "c:\pluto.makerelease\trunk\src\Orbiter\CENet_Smartphone"
call "build_cab.bat"

echo SMARTPHONE_TREO
cd "c:\pluto.makerelease\trunk\src\Orbiter\CENet_Smartphone_Treo"
call "build_cab.bat"

echo XP
cd C:\pluto.makerelease\trunk\installers\Installers\Orbiter
call WinMake.bat
cd C:\pluto.makerelease\trunk\installers\Installers\Orbiter\Release\
copy *.msi C:\pluto.makerelease\trunk\src\bin

echo COPYING LINUXMCE FILES...

cd "c:\pluto.makerelease\trunk\src\bin"


echo Copying EXE files...
copy *.exe "\\10.0.2.4\builds\Windows_Output_LinuxMCE\src\bin"
echo Copying DLL files...
copy *.dll "\\10.0.2.4\builds\Windows_Output_LinuxMCE\src\bin"
echo Copying DAT files...
copy *.dat "\\10.0.2.4\builds\Windows_Output_LinuxMCE\src\bin"
echo Copying SIS files...
copy *.sis "\\10.0.2.4\builds\Windows_Output_LinuxMCE\src\bin"
echo Copying CAB files...
copy *.cab "\\10.0.2.4\builds\Windows_Output_LinuxMCE\src\bin"

echo Copying LIB files...
cd "c:\pluto.makerelease\trunk\src\lib"
copy *.lib "\\10.0.2.4\builds\Windows_Output_LinuxMCE\src\lib"

echo Copying MSI files...
cd C:\pluto.makerelease\trunk\installers\Installers\Orbiter\Release\
copy *.msi "\\10.0.2.4\builds\Windows_Output_LinuxMCE\src\bin"

echo Done. (linuxmce)

echo ==============================================================================================

