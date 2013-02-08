project(plasma-activateorbiter)
 
# Find the required Libaries
find_package(KDE4 REQUIRED)
find_package(Plasma REQUIRED)
include(KDE4Defaults)
 
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
   ..
   ../DCE
   ../pluto_main
   ../Gen_Devices
   /usr/include/mysql
   )

link_directories(
   ../lib
)

ADD_DEFINITIONS(-fPIC -fexceptions -Wall -ggdb3)

# We add our source code here
set(activateorbiter_SRCS plasma-activateorbiter.cpp DB.cpp)
 
# Now make sure all files get to the right place
kde4_add_plugin(plasma_applet_activateorbiter ${activateorbiter_SRCS})
target_link_libraries(plasma_applet_activateorbiter 
                      plasma ${KDE4_KDEUI_LIBS}
		      SerializeClass DCECommon PlutoUtils pthread mysqlclient_r
		      )
 
install(TARGETS plasma_applet_activateorbiter
        DESTINATION ${PLUGIN_INSTALL_DIR})
 
install(FILES plasma-applet-activateorbiter.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})

