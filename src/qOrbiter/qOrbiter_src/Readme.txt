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
FLoor plans - datagrid type
Implement security functions - mostly done

Tickets for reference to submit against.
In an effor to keep the history of changes broken up
#1108 - original ticket - really should avoid any addtional commits to this
#1162 - connection screen and associated tickets like config.xml it reads
#1168 - Documentation - if you just note things, please commit against this. and do leave comments!!!!
#1209 - missing screens
#1170 - Android Platform build notes
#1173 - Qorbiter datagrid behavior
#1177 - File Details screen ticket
#1180 - Filters for media showing all instead of proper filtering types
#1181 - Scenario buttons attemping to pull wrong image names.
#1247 - sleeping menu ticket
#1438 - internal media player
#1822 - Qt5 Android Default skin


##Android building information
versioning codes
08 - api target
1234 - screen sizes, 1-4 for all
10 - version


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

experimental
<<
#D36115
2
<<
#82BDE4
3
<<
#8E1212
4
<<
#F1EBE4
5
<<
#46B1E6

screen handlers info
golgoj5: all the screen handlers are in src/Orbiter/ScreenHandler.cpp and src/Orbiter/Linux/OSDScreenHandler.cpp
