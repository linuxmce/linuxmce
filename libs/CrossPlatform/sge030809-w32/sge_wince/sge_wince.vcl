<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: sge_wince - Win32 (WCE ARMV4) Debug--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\Race\LOCALS~1\Temp\RSPC4B.tmp" with contents
[
/nologo /W3 /Zi /Od /I "c:\pluto/trunk\CrossPlatform-Libs\SDL-1.2.7\include" /I "c:\pluto/trunk\CrossPlatform-Libs\freetype-2.1.9\include" /I "c:\pluto/trunk\CrossPlatform-Libs\SDL_image-1.2.3" /D "DEBUG" /D _WIN32_WCE=420 /D "WIN32_PLATFORM_PSPC=400" /D "ARM" /D "_ARM_" /D "ARMV4" /D UNDER_CE=420 /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fp"ARMV4Dbg/sge_wince.pch" /YX /Fo"ARMV4Dbg/" /Fd"ARMV4Dbg/" /MC /c 
"C:\PLUTO\TRUNK\CROSSPLATFORM-LIBS\SGE030809-W32\src\sge_bm_text.cpp"
"C:\PLUTO\TRUNK\CROSSPLATFORM-LIBS\SGE030809-W32\src\sge_textpp.cpp"
"C:\PLUTO\TRUNK\CROSSPLATFORM-LIBS\SGE030809-W32\src\sge_tt_text.cpp"
]
Creating command line "clarm.exe @C:\DOCUME~1\Race\LOCALS~1\Temp\RSPC4B.tmp" 
Creating temporary file "C:\DOCUME~1\Race\LOCALS~1\Temp\RSPC4C.tmp" with contents
[
/nologo /out:"ARMV4Dbg\sge_wince.lib" 
".\ARMV4Dbg\sge_blib.obj"
".\ARMV4Dbg\sge_bm_text.obj"
".\ARMV4Dbg\sge_collision.obj"
".\ARMV4Dbg\sge_misc.obj"
".\ARMV4Dbg\sge_primitives.obj"
".\ARMV4Dbg\sge_rotation.obj"
".\ARMV4Dbg\sge_shape.obj"
".\ARMV4Dbg\sge_surface.obj"
".\ARMV4Dbg\sge_textpp.obj"
".\ARMV4Dbg\sge_tt_text.obj"
]
Creating command line "link.exe -lib @C:\DOCUME~1\Race\LOCALS~1\Temp\RSPC4C.tmp"
<h3>Output Window</h3>
Compiling...
sge_bm_text.cpp
c:\program files\windows ce tools\wce420\pocket pc 2003\include\armv4\xstring(724) : warning C4530: C++ exception handler used, but unwind semantics are not enabled. Specify -GX
        c:\program files\windows ce tools\wce420\pocket pc 2003\include\armv4\xstring(720) : while compiling class-template member function 'void __cdecl std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >::_Copy(unsigned int)'
sge_textpp.cpp
c:\program files\windows ce tools\wce420\pocket pc 2003\include\armv4\xstring(724) : warning C4530: C++ exception handler used, but unwind semantics are not enabled. Specify -GX
        c:\program files\windows ce tools\wce420\pocket pc 2003\include\armv4\xstring(720) : while compiling class-template member function 'void __cdecl std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >::_Copy(unsigned int)'
sge_tt_text.cpp
c:\program files\windows ce tools\wce420\pocket pc 2003\include\armv4\xstring(724) : warning C4530: C++ exception handler used, but unwind semantics are not enabled. Specify -GX
        c:\program files\windows ce tools\wce420\pocket pc 2003\include\armv4\xstring(720) : while compiling class-template member function 'void __cdecl std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >::_Copy(unsigned int)'
Creating library...




<h3>Results</h3>
sge_wince.lib - 0 error(s), 3 warning(s)
</pre>
</body>
</html>
