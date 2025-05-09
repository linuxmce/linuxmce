
 Read.me for release 1.09b (revised)
 ==================================


 THIS SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.

 IN NO EVENT SHALL MARTI MARIA BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
 INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
 LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 OF THIS SOFTWARE.

 The lcms library is now distributed under

                GNU LESSER GENERAL PUBLIC LICENSE

 See file COPYING. for details


   This is the 8th. public release the engine. It has been tested 
   across several versions before, but it is possible some
   bugs still arises. If so, sorry for the inconvenience, and
   please feel free to submit any suggestion/solution (if you can
   found it) at:

                       marti@littlecms.com

 
   Note that the aesthetics of resulting colors are due only to
   profiles, and not as consequence of the lcms package.

   The main site for the package is located at

           http://www.littlecms.com
        or
               http://www.lcms.coloraid.de


   Littlecms has also a mailing list on:

   http://lists.sourceforge.net/lists/listinfo/lcms-user


   Looking forward the lcms project would grow in future, I will 
   welcome any contribution/optimization/enhancement.

   Enjoy!

 New in ver 1.09
 ===============

  Overall faster speed.
  Much better absolute colorimetric intents. LittleCMS does now 'undo' 
    chromatic adaptation as recommended by latest ICC spec.
  Full softproofing.
  Full gamutchecking.
  Writting capabilities. Lcms can now create profiles as well.
  New CIE94, CMC and BFD color difference spaces.
  Low-level LUT handling is now promoted to API and documented.
  Parametric curves.
  Several ICC 4.0 extensions. Full ICC 4.0 compliance is expected on next revision.
  Improved WhiteBlack compensation
  Improved portability. Several new platforms added.
  New JPEGICC utility. 
  Improved TIFFICC and ICCTRANS utilities. 
  Some minor bugs fixed.



  Configuration
  =============

  All configuration is done by commenting/uncommenting a set
  of toggles placed on the beginning of lcms.h

  Normally, there is no need to touch anything. Only make sure
  to set properly  the NON_WINDOWS toggle if you are using the 
  engine in non MS-Windows environtments, and USE_BIG_ENDIAN if
  your machine does use this convention. Remaining toggles does
  control optimization degree. The testebed program is supposed
  to give some clues if anything is wrongly configured.

   
  Compiler support
  ================

   Following C compilers are tested and worked Ok.

   gcc 2.9x
   gcc 3.x
   cc
   Borland C++ 4.5
   Borland C++ 5.xx
   Borland C++ Builder (all versions)
   VisualC++ 5
   VisualC++ 6
   VisualC++ 7
   Watcom
   CodeWarrior


  ANSI C / C99
  ============

   I have recived some complains about the claim lcms is written
   in ANSI-C. The code is NOT ANSI C in the way you cannot drop
   it directly to any ANSI C compiler and expect it will work on
   first try. But if you would like to port anyway, first take a
   look on lcms.h You should use the NON_WINDOWS toggle for activate
   alternate defines. Syntax of code is very close to ANSI-C.
   Check also USE_BIG_ENDIAN toggle if your target machine works
   in such way. I will be glad to know if it worked on any exotic
   environment.

  

   Delphi
   ======

   Delphi should also have the ability of compile the engine,
   since it includes BC C 5.4 as a command-line version. So, in
   theory, despite I have not been able to check it directly, it
   should work. Delphi access to the engine can be easely
   accomplished by using the DLL and the supplied unit LCMSDLL.PAS

   See the Delphi directory for more details. 



   GCC/linux
   ==========

   Thanks to Karl Heinz Kremer, since ver 1.06, lcms does compile 
   under gcc in linux environments. For compatibility sake, the 
   fixed point  math primitives are implemented using floating point.
   Obviously this can slow down the transforms. If your machine
   does support the new __int64 or "long long" type, you can gain 
   speed by defining the USE_INT64 toggle. There is a separate tarball
   holding the linux redistributable. See INSTALL file for more info.


   Other languages (windows)
   =========================

   Using the DLL, you could access the engine from almost any
   language in Win32 environments.

   If your compiler has IMPLIB.EXE import librarian, you can
   generate your own lib by extracting entry points directly from
   LCMS.DLL

   If this fails, and your linker does accept .DEF files you can
   try to include the definition file on linking step

                          LCMSDEF.DEF


  About profiles
  ==============

    The demo of this package includes some profiles for colorspace
    conversions. I figure all of them are in public domain, but
    since some contains copyright notice, I will enumerate here
    the sources:

         Sun Microsystems Java SDK (widely available)
         Kodak public FTP site: ftp.kodak.com
         ICM Stress demo from microsoft. www.microsoft.com
         sRGB from sRGB site www.srgb.com

    If you found some profile of these not to be in public domain,
    please notify me. I will remove the offending profile as soon as
    posible.



  Additional files
  ================

    ICC34.h is the header file the International Color Consortium
    has posted for version spec 3.4, with some minor modifications
    for improving portability.

    You can reach it at

                http://www.color.org


