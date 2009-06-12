#!/bin/bash
export PATH=~/gnupoc:${PATH}
export EPOCROOT=~/symbian-sdks/s60_30/
bldmake bldfiles
abld build gcce urel

