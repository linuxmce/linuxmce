#!/bin/bash
export PATH=~/gnupoc:${PATH}
export EPOCROOT=~/symbian-sdks/s60_30/
bldmake bldfiles
abld build gcce urel
makesis PlutoMO_3rd.pkg
# makekeys -cert -expdays 3650 mykey.key mycert.cer
signsis PlutoMO_3rd.sis PlutoMO_3rd.sisx mycert.cer mykey.key
