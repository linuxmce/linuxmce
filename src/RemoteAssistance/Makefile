SOURCES = charperiod.c RAshell.c
CC = gcc
CXX = g++
CFLAGS = -O2

all: $(SOURCES:.c=)
	cp $+ ../bin

clean:
	rm -f $(SOURCES:.c=)

install: 
	mkdir -p /usr/pluto/keys
	cp id_dsa_remoteassistance $(DESTPATH)/usr/pluto/keys
	mkdir -p /usr/pluto/bin
	cp charperiod $(DESTPATH)/usr/pluto/bin
	cp SetupRemoteAccess.sh $(DESTPATH)/usr/pluto/bin
	cp RemoteAccess_Tunnel.sh $(DESTPATH)/usr/pluto/bin
	cp RA.sh $(DESTPATH)/usr/pluto/bin
	cp RA_ChangePassword.sh $(DESTPATH)/usr/pluto/bin
	cp RA-handler.sh $(DESTPATH)/usr/pluto/bin
	cp RA_KeepPortsAlive.sh $(DESTPATH)/usr/pluto/bin
