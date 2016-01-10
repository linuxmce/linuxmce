SDL Graphics Extension (SGE)
====================================================================================
http://freshmeat.net/projects/sge/
http://www.etek.chalmers.se/~e8cal1/sge/index.html
http://www.digitalfanatics.org/cal/sge/index.html

This is a binary distribution of SGE r030809 for Win32. Requires DirectX 5 or higher.

All needed libraries are included (everything compiled as dlls).

These libraries uses the MS C-Run-time library "msvcrt.dll", you can get it from 
www.microsoft.com/downloads (do a keyword search for "libraries update") if you 
don't already have it.

- SGE.dll includes support for SFont and TrueType rendering 
  (depends on SDL/FreeType/SDL_Image/JPEG/PNG/Z).

- no_img/SGE.dll includes support for TrueType rendering (depends on SDL/FreeType/Z).

- no_ttf/SGE.dll has no support for SFont or TrueType rendering (depends on SDL).


SGE's headerfiles and documentation can be found in src/. Use the right sge_config.h 
when compiling! All dlls are striped.

See http://www.libsdl.org/Xmingw32/index.html for more information if you want to 
crosscompile SDL and SGE applications on Linux to Win32.

The GnuWin32 project (http://gnuwin32.sourceforge.net/) has binary packages for 
FreeType, Jpeg, Png and Z. These packages can be used with both VisualC and Mingw32. 
Binary packages for SDL and SDL_Image are available at http://www.libsdl.org.

If you want to use the GnuWin32 packages for crosscompiling, then note that you might 
need to rename the dll import library if you want dynamicly linked executables 
(e.g. replace libfreetype.a with libfreetype.dll.a). You will also need the 
"freetype-config" file from SGE's homepage if using FreeType.

If you want VisualC compatible .lib (import) files for linking then please use the 
lib.exe tool included with VisualC:
	lib.exe /VERBOSE /MACHINE:I386 /DEF:SGE.def /OUT:SGE.lib /NAME:SGE
or if using Borland CC:
	implib.exe -f SGE-bcc.lib SGE


Versions:
-SGE r030809
-SDL v1.2.5
-SDL_image v1.2.3
-FreeType v2.1.4
-JPEG lib r6b
-libPNG v1.2.4
-Zlib v1.1.4


SGE is distributed under the terms of the GNU LGPL license (http://www.gnu.org/copyleft/lesser.html).
-Source is included in this package and is also available at http://www.etek.chalmers.se/~e8cal1/sge/index.html.

SDL is distributed under the terms of the GNU LGPL license (http://www.gnu.org/copyleft/lesser.html).
-Source is available at http://www.libsdl.org/.

SDL_image is distributed under the terms of the GNU LGPL license (http://www.gnu.org/copyleft/lesser.html).
-Source is available at http://www.libsdl.org/projects/SDL_image/index.html.

FreeType is distributed under the terms of the FreeType license (http://www.freetype.org/FTL.txt).
-Source is available at http://www.freetype.org.

JPEG lib is distributed under the terms of the JPEGlib license (see README in the jpegsrc package).
-Source is available at http://www.ijg.org/.

libPNG is distributed under the terms of the libPNG license (http://www.libpng.org/pub/png/src/libpng-LICENSE.txt).
-Source is available at http://www.libpng.org/pub/png/libpng.html.

Zlib is distributed under the terms of the Zlib license (http://www.info-zip.org/pub/infozip/zlib/zlib_license.html).
-Source is available at http://www.info-zip.org/pub/infozip/zlib/.

Read src/examples/ttf_readme for license details about the truetype font.

The file sfont.png is copyrighted by Karl Bartel (http://www.linux-games.com/), please 
ask him before including it into any non-(L)GPL project!
