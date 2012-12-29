shfiles = Config_Ops.sh Spawn_Device.sh Start_Bluetooth_Dongle.sh Start_DCERouter.sh Start_Orbiter.sh Start_X.sh Start_ratpoison.sh Startup_Scripts.sh
files = $(shfiles) pluto.func

all:
	for i in $(files); do ln -sf "../BootScripts/$$i" ../bin; done

install: 
	mkdir -p $(DESTDIR)/usr/pluto/bin $(DESTDIR)/usr/pluto/database
	cp update2utf8.pl $(DESTDIR)/usr/pluto/bin

	for i in $(files); do cp "$$i" $(DESTDIR)/usr/pluto/bin || :; done

	# The database dumps are outputs of sqlCVS and need to be provided
	# before, otherwise this will fail.
#	cp constants.sqlcvs $(DESTDIR)/usr/pluto/database || :
#	cp dce.sqlcvs $(DESTDIR)/usr/pluto/database || :
#	cp designer.sqlcvs $(DESTDIR)/usr/pluto/database || :
#	cp document.sqlcvs $(DESTDIR)/usr/pluto/database || :
#	cp ir.sqlcvs $(DESTDIR)/usr/pluto/database || :
#	cp website.sqlcvs $(DESTDIR)/usr/pluto/database || :
#	cp mkr_wpostinst_system.bat $(DESTDIR)/usr/pluto/database
#	cp city.dump $(DESTDIR)/usr/pluto/database || :
