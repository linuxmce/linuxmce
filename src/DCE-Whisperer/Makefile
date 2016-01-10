export DCE_PATH ?= $(shell pwd)/../../src
all: build

build:
ifeq ($(DCE_PATH),)
	@echo "DCE_PATH is not defined"
	@exit 1
endif
	$(MAKE) -C src

install:
	mkdir -p "$(DESTDIR)/usr/pluto/bin"
	cp src/DCE-Whisperer "$(DESTDIR)/usr/pluto/bin"
	cp framework/DCE-connect.sh "$(DESTDIR)/usr/pluto/bin"
