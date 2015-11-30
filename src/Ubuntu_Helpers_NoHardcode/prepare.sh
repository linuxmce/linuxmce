#!/bin/bash

set -e

prepare_scripts_dir="/usr/local/lmce-build/prepare-scripts"

#"${prepare_scripts_dir}/cd1-prepare.sh"
#"${prepare_scripts_dir}/cd2-prepare.sh"
"${prepare_scripts_dir}/install-build-pkgs.sh"
#"${prepare_scripts_dir}/create-diskless-debootstrap.sh"
"${prepare_scripts_dir}/import-external-files.sh"
"${prepare_scripts_dir}/preseed-build-pkgs.sh"

