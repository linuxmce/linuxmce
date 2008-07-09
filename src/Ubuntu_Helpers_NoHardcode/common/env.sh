lmce_build_conf_dir="/etc/lmce-build"
build_scripts_dir="/usr/local/lmce-build/build-scripts"

# Setup environment
KVER=`grep linux-headers $lmce_build_conf_dir/build-packages | cut -b 15-99`
KVER_SHORT=$(echo $KVER | cut -d '-' -f1)
KVER_VERY_SHORT=$(echo $KVER | cut -d '.' -f1).$(echo $KVER | cut -d '.' -f2)
KDIR=/usr/src/linux-source-$(echo $KVER | cut -d '-' -f1)
export KVER
export KVER_SHORT
export KVER_VERY_SHORT
export KDIR

. $lmce_build_conf_dir/builder.conf

