del c:\$Installers\$OrbiterCE_x86\*.dll /Q
del c:\$Installers\$OrbiterCE_x86\*.exe /Q

cd
cd ..\..\bin
copy Orbiter_CeNet4_x86.exe "c:\$Installers\$OrbiterCE_x86\"

cd "..\Orbiter\CENet\Misc files"
copy PthreadsCE_x86.dll c:\$Installers\$OrbiterCE_x86\
cd ..\..\Orbiter_CeNet4_x86

"C:\Program Files\Windows CE Tools\wce420\POCKET PC 2003\Tools\Cabwiz.exe" Orbiter_CeNet4_x86.inf /cpu x86

ren Orbiter_CeNet4_x86.x86.CAB Orbiter_CeNet4_x86.CAB
move Orbiter_CeNet4_x86.CAB "..\..\bin"