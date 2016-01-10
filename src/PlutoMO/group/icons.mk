ZDIR=$(EPOCROOT)epoc32\data\z

TARGETDIR=\root\symbian-sdks\s60_30\epoc32\data\z\resource\Apps
ICONTARGETFILENAME=$(TARGETDIR)\PlutoMO.mif

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE :	
	mifconv $(ICONTARGETFILENAME) \
		/c32 ..\gfx\test.svg

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
