call bldmake bldfiles
call abld build armi urel
call makesis PlutoMO.pkg
copy PlutoMO.sis "../../bin/Pluto_S60.sis"
del PlutoMO.sis