del "c:\$Installers\$OrbiterSmartphone_Treo\*.exe" /Q
del "c:\$Installers\$OrbiterSmartphone_Treo\*.dll" /Q

cd ..\..\bin
copy OrbiterTreo.exe "c:\$Installers\$OrbiterSmartphone_Treo\"
cd ..\Orbiter\CENet_Smartphone_Treo

copy ..\..\..\libs\Libraries\pthreads-snap-2002-11-04\PThreadsCE\ARMV4IRel\PthreadsCE.dll "c:\$Installers\$OrbiterSmartphone_Treo\"
copy Misc\Logo.gif "c:\$Installers\$OrbiterSmartphone_Treo\"

"C:\Program Files\Microsoft Visual Studio 8\smartdevices\sdk\sdktools\cabwiz.exe" Orbiter_Treo.inf

move Orbiter_Treo.cab "..\..\bin"