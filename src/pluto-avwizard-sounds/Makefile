all:
	echo make all

install: 
	mkdir -p sounds-temp
	cd sounds-temp ; tar xvfz ../pluto-avwizard-sounds.tar.gz ; cd ..
	cd sounds-temp ; mv home $(DESTDIR)/
	rm -rf sounds-temp

clean:
	echo make clean
