del c:\$Installers\$OrbiterCE\*.dll /Q
del c:\$Installers\$OrbiterCE\*.exe /Q

cd
cd ..\..\bin
copy Orbiter_CeNet4_XScale.exe "c:\$Installers\$OrbiterCE\"
cd "..\Orbiter\CENet\Misc files"

cd Misc
copy AYGSHELL.DLL c:\$Installers\$OrbiterCE\
copy PthreadsCE.dll c:\$Installers\$OrbiterCE\
cd ..\..\CENet_PocketFrog

"C:\Program Files\Windows CE Tools\wce420\POCKET PC 2003\Tools\Cabwiz.exe" Orbiter_CeNet4_XScale.inf

move Orbiter_CeNet4_XScale.CAB "..\..\bin"