#!/bin/make

none:


clean:


install:
	find ./home -iname "*.mp3" -exec install -m644 -D {} $(DESTDIR)/{} \;
	find ./home -iname "*.wav" -exec install -m644 -D {} $(DESTDIR)/{} \;
	find ./home -iname "*.ac3" -exec install -m644 -D {} $(DESTDIR)/{} \;
