/sbin/mkfs.vfat -F 32 -n boot /dev/sdc1
mount /dev/sdc1 /1
cd rpi-gpu/
cp * /1/
cd ..
cp rpi-out/u-boot.bin /1/kernel.img
umount /1
sync
