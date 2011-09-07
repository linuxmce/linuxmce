Readmefile

Details for qOrbiter


|---Setup---|
This assumes you have read and understood the wiki on this topic

You need an existing non-osd orbiter in your device tree. if you do not have one to spare, please just use the
qOrbiter template. Add the device, reload

From qtcreator (with the SDK, _NO_ exceptions) fill in the device number under "run settings". i.e. "-d 35"

In the projects tab, please add "CONFIG+=for_platform" to to Qmake buildsteps. this will ensure the proper set of
skins is provided at runtime. current scopes are

CONFIG+=for_desktop    :: windows*, mac,linux desktop
CONFIG+=for_freemantle :: maemeo5 devices
CONFIG+=for_harmattan  :: harmattan based meego
CONFIG+=ANDROID        :: android
*windows builds not availible

When you compile and run, the device should connect and the proper skins should be included.


|---Stuff Left to Do---|
FLoor plans - both datagrid and graphic type
Local file that save critical runtime functions like router ip, device number
Proper functions for reloading the router, trying to reconnect

Implement floorplans
Implement security functions
Implement functions on file details screen


Tickets for reference to submit against.
In an effor to keep the history of changes broken up
#1108 - original ticket - really should avoid any addtional commits to this
#1167 - ui update issues, for grids, buttons, anything
#1162 - connection screen and associated tickets like config.xml it reads
#1168 - Documentation - if you just note things, please commit against this. and do leave comments!!!!
#1170 - Android Platform build notes
#1173 - Qorbiter datagrid behavior
#1174 - MacOSX - not finding qml
#1177 - File Details screen ticket
#1183 - Now Playing Button
#1179 - Room List showing all rooms, but not all selectable
#1180 - Filters for media showing all instead of proper filtering types
#1181 - Scenario buttons attemping to pull wrong image names.

If something you are working on is not described here, please open a ticket for it so we can track that specific issue.

/golgoj4 pet project area
//color schemes
defualt -
bg - midnightblue
highlight1 - lightsteelblue
highlight2 - slategrey
highlight3 - aliceblue

noir-
bg -black
highlight1 - darkseagreen
highlight2 - darkolivegreen
highlight3 - oldlace
