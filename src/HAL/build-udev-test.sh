gcc -o udev-test udev-test.c udi-helper.c -Wall -ludev -g `pkg-config --cflags --libs glib-2.0`
