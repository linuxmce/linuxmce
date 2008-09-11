
LIBSRCS += hdhomerun_pkt.c
LIBSRCS += hdhomerun_discover.c
LIBSRCS += hdhomerun_control.c
LIBSRCS += hdhomerun_channelscan.c
LIBSRCS += hdhomerun_video.c
LIBSRCS += hdhomerun_device.c

CFLAGS += -Wall -O2 -Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes -Wpointer-arith
CPPFLAGS += -Wall -O2

all: ../bin/hdhomerun_config ../bin/hdhomerun_mythconfig

hdhomerun_config : $(LIBSRCS) hdhomerun_config.c
	$(CC) $(CFLAGS) $(LIBSRCS) hdhomerun_config.c -lpthread -o $@
	strip $@

hdhomerun_config.exe : $(LIBSRCS) hdhomerun_config.c
	$(CC) $(CFLAGS) $(LIBSRCS) hdhomerun_config.c -lpthread -liphlpapi -o $@
	strip $@

hdhomerun_mythconfig : $(LIBSRCS) hdhomerun_mythconfig.cpp
	$(CXX) $(CPPFLAGS) $(LIBSRCS) hdhomerun_mythconfig.cpp -lpthread -lmysqlclient -o $@
	strip $@

../bin/hdhomerun_config: hdhomerun_config
	cp $< $@

../bin/hdhomerun_config.exe: hdhomerun_config.exe
	cp $< $@

../bin/hdhomerun_mythconfig: hdhomerun_mythconfig
	cp $< $@

clean :
	rm -f hdhomerun_config
	rm -f hdhomerun_config.exe
	rm -f hdhomerun_mythconfig
