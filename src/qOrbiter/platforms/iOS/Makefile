#############################################################################
# Makefile to generate moc files to be included in XCode
# Author: Serge Wagener
#############################################################################

MOC		= /users/sergewagener/Qt/qt-lighthouse-ios-simulator/bin/moc
DEFINES = -Dfor_desktop -DQT_DECLARATIVE_LIB -DQT_GUI_LIB -DQT_CORE_LIB
INCPATH = -I.

all:

	$(MOC) $(DEFINES) $(INCPATH) ../../Gen_Devices/qOrbiterBase.h -o moc_files/moc_qOrbiterBase.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter.h -o moc_files/moc_qOrbiter.cpp

	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/avcodegrid.h -o moc_files/moc_avcodegrid.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/avitem.h -o moc_files/moc_avitem.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/orbiterwindow.h -o moc_files/moc_orbiterwindow.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/qorbitermanager.h -o moc_files/moc_qorbitermanager.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/qOrbiterData.h -o moc_files/moc_qOrbiterData.cpp

	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/contextobjects/epgchannellist.h -o moc_files/moc_epgchannellist.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/contextobjects/filedetailsclass.h -o moc_files/moc_filedetailsclass.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/contextobjects/floorplandevice.h -o moc_files/moc_floorplandevice.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/contextobjects/nowplayingclass.h -o moc_files/moc_nowplayingclass.cpp
#	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/contextobjects/playlistclass.h -o moc_files/moc_playlistclass.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/contextobjects/playlistitemclass.h -o moc_files/moc_playlistitemclass.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/contextobjects/screenparamsclass.h -o moc_files/moc_screenparamsclass.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/contextobjects/screenshotattributes.h -o moc_files/moc_screenshotattributes.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/contextobjects/securityvideoclass.h -o moc_files/moc_securityvideoclass.cpp
	
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/attributemodel.h -o moc_files/moc_attributemodel.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/attributesortmodel.h -o moc_files/moc_attributesortmodel.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/climatescenarioitem.h -o moc_files/moc_climatescenarioitem.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/climatescenariomodel.h -o moc_files/moc_climatescenariomodel.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/filedetailsmodel.h -o moc_files/moc_filedetailsmodel.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/filtermodel.h -o moc_files/moc_filtermodel.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/floorplanimageitem.h -o moc_files/moc_floorplanimageitem.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/floorplanmodel.h -o moc_files/moc_floorplanmodel.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/genremodel.h -o moc_files/moc_genremodel.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/griditem.h -o moc_files/moc_griditem.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/lightingscenarioitem.h -o moc_files/moc_lightingscenarioitem.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/lightingscenariomodel.h -o moc_files/moc_lightingscenariomodel.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/listModel.h -o moc_files/moc_listModel.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/locationitem.h -o moc_files/moc_locationitem.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/locationmodel.h -o moc_files/moc_locationmodel.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/mediascenarioitem.h -o moc_files/moc_mediascenarioitem.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/mediascenariomodel.h -o moc_files/moc_mediascenariomodel.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/mediatypemodel.h -o moc_files/moc_mediatypemodel.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/orbiterbuttonitem.h -o moc_files/moc_orbiterbuttonitem.cpp
#	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/orbiteritem.h -o moc_files/moc_orbiteritem.cpp
#	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/orbitermodel.h -o moc_files/moc_orbitermodel.cpp
#	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/orbiterrowmodel.h -o moc_files/moc_orbiterrowmodel.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/securityscenarioitem.h -o moc_files/moc_securityscenarioitem.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/securityscenariomodel.h -o moc_files/moc_securityscenariomodel.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/skindataitem.h -o moc_files/moc_skindataitem.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/skindatamodel.h -o moc_files/moc_skindatamodel.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/telecomscenarioitem.h -o moc_files/moc_telecomscenarioitem.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/telecomscenariomodel.h -o moc_files/moc_telecomscenariomodel.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/useritem.h -o moc_files/moc_useritem.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/usermodel.h -o moc_files/moc_usermodel.cpp

	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/DatagridClasses/datagriditem.h -o moc_files/moc_datagriditem.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/DatagridClasses/datagriditemmodelclass.h -o moc_files/moc_datagriditemmodelclass.cpp

	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/DataModelItems/attributesortitem.h -o moc_files/moc_attributesortitem.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/DataModelItems/filedetailsitem.h -o moc_files/moc_filedetailsitem.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/DataModelItems/filtermodelitem.h -o moc_files/moc_filtermodelitem.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/DataModelItems/genreitem.h -o moc_files/moc_genreitem.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/DataModelItems/mediatypeitem.h -o moc_files/moc_mediatypeitem.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/datamodels/DataModelItems/sleepingalarm.h -o moc_files/moc_sleepingalarm.cpp

	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/imageProviders/abstractimageprovider.h -o moc_files/moc_abstractimageprovider.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/imageProviders/basicimageprovider.h -o moc_files/moc_basicimageprovider.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/imageProviders/filedetailsimageprovider.h -o moc_files/moc_filedetailsimageprovider.cpp
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/imageProviders/gridimageprovider.h -o moc_files/moc_gridimageprovider.cpp

	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/playlists/epgitemclass.h -o moc_files/moc_epgitemclass.cpp
	
	$(MOC) $(DEFINES) $(INCPATH) ../qOrbiter_src/screensaver/screensavermodule.h -o moc_files/moc_screensavermodule.cpp