SET PATH="c:\Program Files\Microsoft eMbedded C++ 4.0\Common\EVC\Bin"

CALL SET CONFIGURATION=release
CALL SET INPUT_PATH=c:\pluto\trunk
CALL SET MAKERELEASE_PATH=C:\makerelease
CALL SET LOG_PATH=c:\@build_log

del %LOG_PATH%\BuildCE.log

%MAKERELEASE_PATH%\MakeRelease.exe -h 192.168.80.1 -o 12 -s %INPUT_PATH% -n / -c -k 119,138

cd "Orbiter\CENet_Smartphone"
call build_cab.bat
cd "..\CENet\"
call build_cab.bat
cd "..\CENet_PocketFrog"
call build_cab.bat
cd "..\Orbiter_CeNet4_x86"
call build_cab.bat
cd "..\CENet_Smartphone_Treo"
call build_cab.bat












