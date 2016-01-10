@echo off
bcc32 @lcms109.lst
if errorlevel 0 tlib @lcms109.cmd
del *.obj
echo Done!
								
					
