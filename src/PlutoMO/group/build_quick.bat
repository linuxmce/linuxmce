call bldmake bldfiles
call abld build armi urel
call makesis PlutoMO_quick.pkg
copy PlutoMO_quick.sis "../../bin/Pluto_S60.sis"
del PlutoMO_quick.sis