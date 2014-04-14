#!/bin/sh

cd hald
P="`pwd`"
export HALD_RUNNER_PATH=$P/linux:$P/linux/probing:$P/linux/addons:.:../tools:../tools/linux
export PATH=../hald-runner:$PATH
export HAL_FDI_SOURCE_PREPROBE=../fdi/preprobe
export HAL_FDI_SOURCE_INFORMATION=../fdi/information
export HAL_FDI_SOURCE_POLICY=../fdi/policy
./hald --daemon=no --verbose=yes

