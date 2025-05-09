This document contains instructions how to build the FreeType library on
non-Unix  systems with  the help  of  GNU Make.   Note that  if you  are
running Cygwin or MSys in Windows, you should follow the instructions in
the file INSTALL.UNX instead.


  FreeType 2 includes  a powerful and flexible build  system that allows
  you to  easily compile  it on  a great variety  of platforms  from the
  command line.  To do so, just follow these simple instructions:

  1. Install GNU Make
  -------------------

    Because  GNU  Make  is  the  only Make  tool  supported  to  compile
    FreeType 2, you should install it on your machine.

    The FreeType 2  build system relies on many  features special to GNU
    Make --  trying to build the  library with any other  Make tool will
    *fail*.

    NEARLY  ALL OTHER  MAKE TOOLS  WILL FAIL,  INCLUDING "BSD  MAKE", SO
    REALLY INSTALL A RECENT VERSION OF GNU MAKE ON YOUR SYSTEM!

    Note  that make++,  a make  tool  written in  Perl, supports  enough
    features    of    GNU     make    to    compile    FreeType.     See
    http://makepp.sourceforge.net for more information; you need version
    1.19 or newer, and you must pass option `--norc-substitution'.

    Make sure that  you are invoking GNU Make from  the command line, by
    typing something like:

      make -v

    to display its version number.

    VERSION 3.78.1 OR NEWER IS NEEDED!


  2. Invoke 'make'
  ----------------

    Go to the root directory of  FreeType 2, then simply invoke GNU Make
    from  the  command line.   This  will  launch  the FreeType  2  host
    platform  detection  routines.  A  summary  will  be displayed,  for
    example, on Win32:


      ==============================================================
      FreeType build system -- automatic system detection

      The following settings are used:

        platform                     win32
        compiler                     gcc
        configuration directory      ./builds/win32
        configuration rules          ./builds/win32/w32-gcc.mk

      If this does not correspond to your system or settings please
      remove the file 'config.mk' from this directory then read the
      INSTALL file for help.

      Otherwise, simply type 'make' again to build the library.
      =============================================================


    If the  detected settings correspond to your  platform and compiler,
    skip to step  5.  Note that if your platform  is completely alien to
    the build system, the detected platform will be 'ansi'.


  3. Configure the build system for a different compiler
  ------------------------------------------------------

    If the build  system correctly detected your platform,  but you want
    to use  a different compiler than  the one specified  in the summary
    (for most  platforms, gcc is  the defaut compiler), invoke  GNU Make
    with

      make setup <compiler>

    Examples:

      to use Visual C++ on Win32, type:  "make setup visualc"
      to use Borland C++ on Win32, type  "make setup bcc32"
      to use Watcom C++ on Win32, type   "make setup watcom"
      to use Intel C++ on Win32, type    "make setup intelc"
      to use LCC-Win32 on Win32, type:   "make setup lcc"
      to use Watcom C++ on OS/2, type    "make setup watcom"
      to use VisualAge C++ on OS/2, type "make setup visualage"

    The  <compiler> name  to  use is  platform-dependent.   The list  of
    available  compilers  for  your  system  is available  in  the  file
    `builds/<system>/detect.mk'

    If  you are  satisfied by  the  new configuration  summary, skip  to
    step 5.


  4. Configure the build system for an unknown platform/compiler
  --------------------------------------------------------------

    The auto-detection/setup phase of the  build system copies a file to
    the current directory under the name `config.mk'.

    For    example,    on     OS/2+gcc,    it    would    simply    copy
    `builds/os2/os2-gcc.mk' to `./config.mk'.

    If  for some  reason your  platform isn't  correctly  detected, copy
    manually the  configuration sub-makefile to `./config.mk'  and go to
    step 5.

    Note that this file is a sub-Makefile used to specify Make variables
    for compiler and linker invocation during the build.  You can easily
    create  your own  version  from one  of  the existing  configuration
    files,  then  copy  it  to  the current  directory  under  the  name
    `./config.mk'.


  5. Build the library
  --------------------

    The  auto-detection/setup phase  should have  copied a  file  in the
    current  directory,   called  `./config.mk'.   This   file  contains
    definitions of  various Make variables  used to invoke  the compiler
    and linker during the build.

    To launch the build, simply  invoke GNU Make again: The top Makefile
    will detect the configuration file and run the build with it.


  Final note
  
    The  build system  builds a  statically linked  library of  the font
    engine in the "objs" directory.   It does _not_ support the build of
    DLLs on Windows and OS/2.  If you need these, you have to either use
    a  IDE-specific   project  file,  or  follow   the  instructions  in
    "INSTALL.ANY" to create your own Makefiles.


--- end of INSTALL.GNU ---
