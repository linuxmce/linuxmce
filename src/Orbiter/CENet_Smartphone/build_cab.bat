del "c:\$Installers\$OrbiterSmartphone\*.exe" /Q
del "c:\$Installers\$OrbiterSmartphone\*.dll" /Q

cd ..\..\bin
copy OrbiterSmartphone.exe "c:\$Installers\$OrbiterSmartphone\"
cd ..\Orbiter\CENet_Smartphone

cd Misc
copy AYGSHELL.DLL "c:\$Installers\$OrbiterSmartphone\"
copy PthreadsCE.dll "c:\$Installers\$OrbiterSmartphone\"
cd ..

"C:\Program Files\Windows CE Tools\wce420\SMARTPHONE 2003\Tools\CabwizSP.exe" Orbiter.inf

move Orbiter.cab "..\..\bin"