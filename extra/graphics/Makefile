#!/bin/make

none:


clean:


install:
	find . -iname "*.png" -exec install -m644 -D {} $(DESTDIR)/usr/pluto/orbiter/skins/{} \;
	find . -iname "*.mng" -exec install -m644 -D {} $(DESTDIR)/usr/pluto/orbiter/skins/{} \;
	find . \( -name "*.xbm" , -name "*.xbm.msk" \) -exec install -m644 -D {} $(DESTDIR)/usr/pluto/orbiter/skins/{} \;

	cd ./Basic/avwizard/images/ && \
	find . \( -name "*.png" -o -name "*.xml" \) -exec install -m644 -D {} $(DESTDIR)/usr/pluto/wiz_pixmaps/{} \; && \
	cd ../../../

