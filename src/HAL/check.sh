#!/bin/bash
./udev-test > out.txt
hal-device > outhal.txt
cat out.txt | grep -i udi: | cut -d' ' -f 5 | sort > z.udev
cat outhal.txt | grep -i " udi =" | cut -d"'" -f2 | sort > z.hal
diff z.udev z.hal
