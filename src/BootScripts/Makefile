shfiles = Config_Ops.sh Spawn_Device.sh Start_Bluetooth_Dongle.sh Start_DCERouter.sh Start_Orbiter.sh Start_X.sh Start_ratpoison.sh Startup_Scripts.sh
files = $(shfiles) pluto.func

all:
	for i in $(files); do ln -sf "../BootScripts/$$i" ../bin; done

install: 
	mkdir -p /usr/pluto/bin
	cp update2utf8.pl $(DESTPATH)/usr/pluto/bin
	mkdir -p pluto/database
	cp constants.sqlcvs $(DESTPATH)pluto/database
	cp dce.sqlcvs $(DESTPATH)pluto/database
	cp designer.sqlcvs $(DESTPATH)pluto/database
	cp document.sqlcvs $(DESTPATH)pluto/database
	cp ir.sqlcvs $(DESTPATH)pluto/database
	cp website.sqlcvs $(DESTPATH)pluto/database
	cp mkr_wpostinst_system.bat $(DESTPATH)pluto/database
	cp city.dump $(DESTPATH)pluto/database
