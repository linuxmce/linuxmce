DCE_PATH=../..

all: build

build:
	$(MAKE) -C src

install:
	mkdir -p "$(DESTDIR)/usr/pluto/bin"
	cp src/DCE-Whisperer "$(DESTDIR)/usr/pluto/bin"
	cp framework/DCE-connect.sh "$(DESTDIR)/usr/pluto/bin"
