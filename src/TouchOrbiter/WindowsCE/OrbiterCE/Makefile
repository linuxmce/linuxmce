BaseDir=..

CPPFLAGS=-I $(BaseDir)/zlib-1.2.5 -I $(BaseDir)/libpng-1.4.8 -I $(BaseDir)/jpeg-6b-src/src/jpeg-6b-src -I $(BaseDir)/jpeg-6b-src/src/jpeg-6b
CPPFLAGS+=-D_WIN32_WCE=0x0500
CFLAGS=-W -Wall
LDFLAGS=-L$(BaseDir)/zlib-1.2.5 -L$(BaseDir)/libpng-1.4.8 -L$(BaseDir)/jpeg-6b-src/src/jpeg-6b-src
LDLIBS=-lpng -lz -lcommctrl -lws2 -laygshell -limgdecmp -ljpeg

%.rsc: %.rc
	$(CROSS)windres $< $@

%.o: %.c
	$(CROSS)gcc $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

Objects=OrbiterCE.o OrbiterCE.rsc PngFile.o JpegFile.o ProxyOrbiter.o TransformBitmap.o WaitingCursor.o IniFile.o

OrbiterCE.exe: $(Objects)
	$(CROSS)gcc $(CFLAGS) -o $@ $+ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f OrbiterCE.exe *.o *.rsc
