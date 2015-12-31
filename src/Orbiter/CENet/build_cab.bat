del c:\$Installers\$OrbiterCE_SDL\*.dll /Q
del c:\$Installers\$OrbiterCE_SDL\*.exe /Q

cd
cd ..\..\bin
copy OrbiterCE_SDL.exe "c:\$Installers\$OrbiterCE_SDL\"
cd "..\Orbiter\CENet\Misc files"

cd Misc
copy AYGSHELL.DLL c:\$Installers\$OrbiterCE_SDL\
copy PthreadsCE.dll c:\$Installers\$OrbiterCE_SDL\
copy SDL.dll c:\$Installers\$OrbiterCE_SDL\
cd ..

"C:\Program Files\Windows CE Tools\wce420\POCKET PC 2003\Tools\Cabwiz.exe" OrbiterCE_SDL.inf

move OrbiterCE_SDL.cab "..\..\bin"