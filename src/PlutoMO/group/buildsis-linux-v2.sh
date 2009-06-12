#!/bin/bash
export PATH=~/gnupoc:${PATH}
export EPOCROOT=~/symbian-sdks/s60_20/
bldmake bldfiles
abld build armi urel
makesis PlutoMO.pkg

