SET PATH="c:\Program Files\Microsoft Visual Studio .NET 2003\Common7\IDE"

CALL SET CONFIGURATION=release
CALL SET INPUT_PATH=c:\pluto\trunk
CALL SET MAKERELEASE_PATH=C:\makerelease
CALL SET LOG_PATH=c:\@build_log

del %LOG_PATH%\Build.log

ren %INPUT_PATH%\src\pluto.sln pluto.sln.temp
%MAKERELEASE_PATH%\MakeRelease.exe -h 192.168.80.1 -o 7 -s %INPUT_PATH% -n / -c -k 192,193,213,212,201,237,255,254,277,278,126,145,119,138,124,143,241,242,154,155,159,160,202,203,303,304,272,281,280,279,195,194,191,190,162,161,128,147,341,342,471,470,346,347
ren %INPUT_PATH%\src\pluto.sln.temp pluto.sln

echo Updating Orbiter release binary for OrbiterInstaller
copy C:\pluto\trunk\src\bin\Orbiter.exe C:\pluto\trunk\installers\Installers\Orbiter\Files\

cd C:\pluto\trunk\installers\Installers\NewOrbiterInstaller\NewOrbiterInstaller
CALL WinMake.bat

cd C:\pluto\trunk\installers\Installers\NewOrbiterInstaller\NewOrbiterInstaller\Release\
copy *.msi C:\pluto\trunk\src\bin






