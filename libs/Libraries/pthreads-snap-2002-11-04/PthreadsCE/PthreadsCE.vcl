<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: PthreadsCE - Win32 (WCE x86) Debug--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\Esprit\LOCALS~1\Temp\RSP1200.tmp" with contents
[
/nologo /W3 /Zi /Od /I "../../Libraries/pthreads-snap-2002-11-04" /I "../.." /I "../../Libraries/Pocketfrog_0_6_0" /I "../../Libraries/STLport-4.5.3/stlport" /I "../../Libraries/zlib" /I "../../Libraries/jpgsrc6b" /I "../../Libraries/libmng" /I "../../Libraries/lcms/include" /D "DEBUG" /D "_i386_" /D "_X86_" /D "x86" /D "WINCE" /D "PTW32_BUILD" /D _WIN32_WCE=400 /D "WCE_PLATFORM_STANDARDSDK" /D UNDER_CE=400 /D "UNICODE" /D "_UNICODE" /D "_USRDLL" /D "PTHREADSCE_EXPORTS" /D "HAVE_CONFIG_H" /FR"X86Dbg/" /Fp"X86Dbg/PthreadsCE.pch" /YX /Fo"X86Dbg/" /Fd"X86Dbg/" /Gs8192 /GF /c 
"C:\pluto\trunk\libs\Libraries\pthreads-snap-2002-11-04\attr.c"
"C:\pluto\trunk\libs\Libraries\pthreads-snap-2002-11-04\cleanup.c"
"C:\pluto\trunk\libs\Libraries\pthreads-snap-2002-11-04\condvar.c"
"C:\pluto\trunk\libs\Libraries\pthreads-snap-2002-11-04\create.c"
"C:\pluto\trunk\libs\Libraries\pthreads-snap-2002-11-04\dll.c"
"C:\pluto\trunk\libs\Libraries\pthreads-snap-2002-11-04\errno.c"
"C:\pluto\trunk\libs\Libraries\pthreads-snap-2002-11-04\fork.c"
"C:\pluto\trunk\libs\Libraries\pthreads-snap-2002-11-04\global.c"
"C:\pluto\trunk\libs\Libraries\pthreads-snap-2002-11-04\misc.c"
"C:\pluto\trunk\libs\Libraries\pthreads-snap-2002-11-04\mutex.c"
"C:\pluto\trunk\libs\Libraries\pthreads-snap-2002-11-04\nonportable.c"
"C:\pluto\trunk\libs\Libraries\pthreads-snap-2002-11-04\private.c"
"C:\pluto\trunk\libs\Libraries\pthreads-snap-2002-11-04\sched.c"
"C:\pluto\trunk\libs\Libraries\pthreads-snap-2002-11-04\semaphore.c"
"C:\pluto\trunk\libs\Libraries\pthreads-snap-2002-11-04\signal.c"
"C:\pluto\trunk\libs\Libraries\pthreads-snap-2002-11-04\sync.c"
"C:\pluto\trunk\libs\Libraries\pthreads-snap-2002-11-04\tsd.c"
]
Creating command line "cl.exe @C:\DOCUME~1\Esprit\LOCALS~1\Temp\RSP1200.tmp" 
Creating temporary file "C:\DOCUME~1\Esprit\LOCALS~1\Temp\RSP1201.tmp" with contents
[
commctrl.lib coredll.lib corelibc.lib /nologo /base:"0x00100000" /stack:0x10000,0x1000 /entry:"_DllMainCRTStartup" /dll /incremental:yes /pdb:"X86Dbg/PthreadsCE_x86.pdb" /debug /nodefaultlib:"OLDNAMES.lib" /nodefaultlib:libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib /out:"X86Dbg/PthreadsCE_x86.dll" /implib:"X86Dbg/PthreadsCE_x86.lib" /subsystem:windowsce,4.00 /MACHINE:IX86 
".\X86Dbg\attr.obj"
".\X86Dbg\cleanup.obj"
".\X86Dbg\condvar.obj"
".\X86Dbg\create.obj"
".\X86Dbg\dll.obj"
".\X86Dbg\errno.obj"
".\X86Dbg\fork.obj"
".\X86Dbg\global.obj"
".\X86Dbg\misc.obj"
".\X86Dbg\mutex.obj"
".\X86Dbg\nonportable.obj"
".\X86Dbg\private.obj"
".\X86Dbg\sched.obj"
".\X86Dbg\semaphore.obj"
".\X86Dbg\signal.obj"
".\X86Dbg\sync.obj"
".\X86Dbg\tsd.obj"
]
Creating command line "link.exe @C:\DOCUME~1\Esprit\LOCALS~1\Temp\RSP1201.tmp"
<h3>Output Window</h3>
Compiling...
attr.c
cleanup.c
condvar.c
create.c
dll.c
errno.c
fork.c
global.c
misc.c
mutex.c
nonportable.c
private.c
sched.c
semaphore.c
signal.c
sync.c
tsd.c
Linking...
   Creating library X86Dbg/PthreadsCE_x86.lib and object X86Dbg/PthreadsCE_x86.exp




<h3>Results</h3>
PthreadsCE_x86.dll - 0 error(s), 0 warning(s)
</pre>
</body>
</html>
