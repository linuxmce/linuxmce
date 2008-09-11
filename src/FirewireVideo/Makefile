

CFLAGS=`pkg-config --cflags libdc1394` -ggdb3
LIBS=`pkg-config --libs libdc1394`

all: bin

bin: FirewireVideo_Radar

FirewireVideo_Radar: 
	$(CC) $(LIBS) $(CFLAGS) FirewireVideo_Radar.c -o FirewireVideo_Radar
	cp $@ ../bin

clean:
	rm -f FirewireVideo_Radar
