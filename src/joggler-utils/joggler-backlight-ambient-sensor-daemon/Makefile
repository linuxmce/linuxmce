CC=gcc
CFLAGS=-c
INSTALL=/usr/bin/install -c
LDFLAGS=
SOURCES=joggler-backlight-ambient-sensor-daemon.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=joggler-backlight-ambient-sensor-daemon

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

install: all
	$(INSTALL) joggler-backlight-ambient-sensor-daemon /usr/bin

clean:
	rm -rf joggler-backlight-ambient-sensor-daemon
