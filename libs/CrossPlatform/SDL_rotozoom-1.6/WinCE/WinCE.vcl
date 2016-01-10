<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: WinCE - Win32 (WCE ARMV4) Debug--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\Race\LOCALS~1\Temp\RSPC1.tmp" with contents
[
/nologo /W3 /Zi /Od /I "/pluto/trunk/CrossPlatform-Libs/SDL-1.2.7/include" /D "DEBUG" /D _WIN32_WCE=420 /D "WIN32_PLATFORM_PSPC=400" /D "ARM" /D "_ARM_" /D "ARMV4" /D UNDER_CE=420 /D "UNICODE" /D "_UNICODE" /D "_LIB" /D "WINCE" /Fp"ARMV4Dbg/WinCE.pch" /YX /Fo"ARMV4Dbg/" /Fd"ARMV4Dbg/" /MC  /c 
"C:\pluto\trunk\CrossPlatform-Libs\SDL_rotozoom-1.6\SDL_rotozoom.c"
]
Creating command line "clarm.exe @C:\DOCUME~1\Race\LOCALS~1\Temp\RSPC1.tmp" 
Creating command line "link.exe -lib /nologo /out:"ARMV4Dbg\sdlrotozoom-wince-pluto-d.lib"  ".\ARMV4Dbg\SDL_rotozoom.obj" "
<h3>Output Window</h3>
Compiling...
SDL_rotozoom.c
c:\pluto\trunk\crossplatform-libs\sdl_rotozoom-1.6\sdl_rotozoom.c(228) : warning C4018: '<' : signed/unsigned mismatch
c:\pluto\trunk\crossplatform-libs\sdl_rotozoom-1.6\sdl_rotozoom.c(237) : warning C4018: '<' : signed/unsigned mismatch
c:\pluto\trunk\crossplatform-libs\sdl_rotozoom-1.6\sdl_rotozoom.c(247) : warning C4018: '<' : signed/unsigned mismatch
c:\pluto\trunk\crossplatform-libs\sdl_rotozoom-1.6\sdl_rotozoom.c(254) : warning C4018: '<' : signed/unsigned mismatch
c:\pluto\trunk\crossplatform-libs\sdl_rotozoom-1.6\sdl_rotozoom.c(267) : warning C4018: '<' : signed/unsigned mismatch
c:\pluto\trunk\crossplatform-libs\sdl_rotozoom-1.6\sdl_rotozoom.c(271) : warning C4018: '<' : signed/unsigned mismatch
Creating library...




<h3>Results</h3>
sdlrotozoom-wince-pluto-d.lib - 0 error(s), 6 warning(s)
</pre>
</body>
</html>
