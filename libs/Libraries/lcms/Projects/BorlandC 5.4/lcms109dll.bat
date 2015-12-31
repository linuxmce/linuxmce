@echo off
echo.
echo This will build the littlecms DLL using Borland C 5.4 compiler.
echo.
echo Press Ctrl-C to abort, or
pause
bcc32 @lcms109dll.lst
if errorlevel 0 tlink32 @lcms109dll.cmd
if errorlevel 0 brc32 -fe..\..\bin\lcms.dll lcms.rc
del *.obj
del *.res
echo Done!
								
					
