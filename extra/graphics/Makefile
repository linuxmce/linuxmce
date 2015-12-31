#!/bin/make

none:


clean:


install:
	find ./Basic -iname "*.png" -exec install -m644 -D {} $(DESTDIR)/usr/pluto/orbiter/skins/{} \;
	find ./Basic -iname "*.mng" -exec install -m644 -D {} $(DESTDIR)/usr/pluto/orbiter/skins/{} \;
	find ./Basic \( -name "*.xbm" , -name "*.xbm.msk" \) -exec install -m644 -D {} $(DESTDIR)/usr/pluto/orbiter/skins/{} \;

	find ./BlueCrystal -iname "*.png" -exec install -m644 -D {} $(DESTDIR)/usr/pluto/orbiter/skins/{} \;
	find ./BlueCrystal -iname "*.mng" -exec install -m644 -D {} $(DESTDIR)/usr/pluto/orbiter/skins/{} \;
	find ./BlueCrystal \( -name "*.xbm" , -name "*.xbm.msk" \) -exec install -m644 -D {} $(DESTDIR)/usr/pluto/orbiter/skins/{} \;

	find ./LinuxMCE -iname "*.png" -exec install -m644 -D {} $(DESTDIR)/usr/pluto/orbiter/skins/{} \;
	find ./LinuxMCE -iname "*.mng" -exec install -m644 -D {} $(DESTDIR)/usr/pluto/orbiter/skins/{} \;
	find ./LinuxMCE \( -name "*.xbm" , -name "*.xbm.msk" \) -exec install -m644 -D {} $(DESTDIR)/usr/pluto/orbiter/skins/{} \;

	find ./Slate -iname "*.png" -exec install -m644 -D {} $(DESTDIR)/usr/pluto/orbiter/skins/{} \;
	find ./Slate -iname "*.mng" -exec install -m644 -D {} $(DESTDIR)/usr/pluto/orbiter/skins/{} \;
	find ./Slate \( -name "*.xbm" , -name "*.xbm.msk" \) -exec install -m644 -D {} $(DESTDIR)/usr/pluto/orbiter/skins/{} \;

	find ./Titanium -iname "*.png" -exec install -m644 -D {} $(DESTDIR)/usr/pluto/orbiter/skins/{} \;
	find ./Titanium -iname "*.mng" -exec install -m644 -D {} $(DESTDIR)/usr/pluto/orbiter/skins/{} \;
	find ./Titanium \( -name "*.xbm" , -name "*.xbm.msk" \) -exec install -m644 -D {} $(DESTDIR)/usr/pluto/orbiter/skins/{} \;

	find ./UIta -iname "*.png" -exec install -m644 -D {} $(DESTDIR)/usr/pluto/orbiter/skins/{} \;
	find ./UIta -iname "*.mng" -exec install -m644 -D {} $(DESTDIR)/usr/pluto/orbiter/skins/{} \;
	find ./UIta \( -name "*.xbm" , -name "*.xbm.msk" \) -exec install -m644 -D {} $(DESTDIR)/usr/pluto/orbiter/skins/{} \;

	cd ./Basic/avwizard/images/ && \
	find . \( -name "*.png" -o -name "*.xml" \) -exec install -m644 -D {} $(DESTDIR)/usr/pluto/wiz_pixmaps/{} \; && \
	cd ../../../

