Readmefile

Details for qOrbiter


|---Setup---|
You need an existing non-osd orbiter in your device tree. if you do not have one to spare, please just use the
qOrbiter template. Add the device, reload

From qtcreator (with the SDK, _NO_ exceptions) fill in the device number under "run settings". i.e. "-d 35"

In the projects tab, please add "CONFIG+=for_platform" to to Qmake buildsteps. this will ensure the proper set of
skins is provided at runtime. current scopes are

CONFIG+=for_desktop    :: windows, mac,linux desktop
CONFIG+=for_freemantle :: maemeo5 devices
CONFIG+=for_harmattan  :: harmattan based meego

When you compile and run, the device should connect and the proper skins should be included.


|---Stuff Left to Do---|
FLoor plans - both datagrid and graphic type
Connection screens for entering router / ip and device number
Local file that save critical runtime functions like router ip, device number
Proper functions for reloading the router, trying to reconnect
