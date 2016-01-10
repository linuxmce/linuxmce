<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: zlib_wince - Win32 (WCE x86) Debug 0x86--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\Esprit\LOCALS~1\Temp\RSP122B.tmp" with contents
[
/nologo /W3 /Zi /Od /D "DEBUG" /D _WIN32_WCE=400 /D "WCE_PLATFORM_STANDARDSDK" /D "_i386_" /D UNDER_CE=400 /D "UNICODE" /D "_UNICODE" /D "_X86_" /D "x86" /D "NO_ERRNO_H" /Fp"X86Debug 0x86/zlib_wince.pch" /YX /Fo"X86Debug 0x86/" /Fd"X86Debug 0x86/" /Gs8192 /MC /c 
"C:\pluto\trunk\libs\CrossPlatform\zlib\adler32.c"
"C:\pluto\trunk\libs\CrossPlatform\zlib\crc32.c"
"C:\pluto\trunk\libs\CrossPlatform\zlib\deflate.c"
"C:\pluto\trunk\libs\CrossPlatform\zlib\contrib\masmx86\gvmat32c.c"
"C:\pluto\trunk\libs\CrossPlatform\zlib\gzio.c"
"C:\pluto\trunk\libs\CrossPlatform\zlib\inffast.c"
"C:\pluto\trunk\libs\CrossPlatform\zlib\inflate.c"
"C:\pluto\trunk\libs\CrossPlatform\zlib\inftrees.c"
"C:\pluto\trunk\libs\CrossPlatform\zlib\trees.c"
"C:\pluto\trunk\libs\CrossPlatform\zlib\uncompr.c"
"C:\pluto\trunk\libs\CrossPlatform\zlib\zutil.c"
]
Creating command line "cl.exe @C:\DOCUME~1\Esprit\LOCALS~1\Temp\RSP122B.tmp" 
Creating temporary file "C:\DOCUME~1\Esprit\LOCALS~1\Temp\RSP122C.tmp" with contents
[
/nologo /out:"X86Debug 0x86\zlib_wince_x86.lib" 
".\X86Debug 0x86\adler32.obj"
".\X86Debug 0x86\crc32.obj"
".\X86Debug 0x86\deflate.obj"
".\X86Debug 0x86\gvmat32c.obj"
".\X86Debug 0x86\gzio.obj"
".\X86Debug 0x86\inffast.obj"
".\X86Debug 0x86\inflate.obj"
".\X86Debug 0x86\inftrees.obj"
".\X86Debug 0x86\trees.obj"
".\X86Debug 0x86\uncompr.obj"
".\X86Debug 0x86\zutil.obj"
]
Creating command line "link.exe -lib @C:\DOCUME~1\Esprit\LOCALS~1\Temp\RSP122C.tmp"
<h3>Output Window</h3>
Compiling...
Command line warning D4002 : ignoring unknown option '/MC'
adler32.c
crc32.c
deflate.c
gvmat32c.c
gzio.c
inffast.c
inflate.c
inftrees.c
trees.c
uncompr.c
zutil.c
Creating library...




<h3>Results</h3>
zlib_wince_x86.lib - 0 error(s), 1 warning(s)
</pre>
</body>
</html>
