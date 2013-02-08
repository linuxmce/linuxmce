PROGRAMS=interactor interactor-server

CPPFLAGS=-g -ggdb3 -W -Wall
CXX=g++
CC=gcc

all: $(PROGRAMS)
	cp $(PROGRAMS) ../bin

interactor-server: interactor-server.o GetCommandOutput.o
	$(CXX) $(CPPFLAGS) -o interactor-server $+

clean:
	rm -f $(PROGRAMS) *.o

install: 
	mkdir -p /etc/init.d
	cp * $(DESTPATH)/etc/init.d
	mkdir -p /etc/initramfs-tools-interactor
	cp * $(DESTPATH)/etc/initramfs-tools-interactor
	mkdir -p /etc/initramfs-tools-interactor/scripts
	cp interactor $(DESTPATH)/etc/initramfs-tools-interactor/scripts
	mkdir -p /home/DisklessFS/
	cp PlutoMD_Debootstraped.tar.bz2 $(DESTPATH)/home/DisklessFS/
	mkdir -p /usr/pluto/bin
	cp Diskless_*.sh $(DESTPATH)/usr/pluto/bin
	cp Disked_*.sh $(DESTPATH)/usr/pluto/bin
	mkdir -p pluto/bin/files.d
	cp * $(DESTPATH)pluto/bin/files.d
