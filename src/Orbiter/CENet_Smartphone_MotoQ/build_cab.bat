del "c:\$Installers\$OrbiterSmartphone_MotoQ\*.exe" /Q
del "c:\$Installers\$OrbiterSmartphone_MotoQ\*.dll" /Q

cd ..\..\bin
copy Orbiter_MotoQ.exe "c:\$Installers\$OrbiterSmartphone_MotoQ\"
cd ..\Orbiter\CENet_Smartphone_MotoQ

copy ..\..\..\libs\Libraries\pthreads-snap-2002-11-04\PThreadsCE\ARMV4IRel\PthreadsCE.dll "c:\$Installers\$OrbiterSmartphone_MotoQ\"
copy Misc\Logo.gif "c:\$Installers\$OrbiterSmartphone_MotoQ\"

"C:\Program Files\Microsoft Visual Studio 8\smartdevices\sdk\sdktools\cabwiz.exe" Orbiter_MotoQ.inf

move Orbiter_MotoQ.cab "..\..\bin"