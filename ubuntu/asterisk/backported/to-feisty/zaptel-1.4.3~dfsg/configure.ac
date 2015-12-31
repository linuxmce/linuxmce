# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)

m4_define([ZAP_VERSION],
          m4_bpatsubst(m4_esyscmd([build_tools/make_version .]),
                       [\([0-9.]*\)\(\w\|\W\)*],
                       [\1]))
AC_INIT(zaptel, ZAP_VERSION, www.asterisk.org)

# check existence of the package
AC_CONFIG_SRCDIR([zaptel-base.c])

AC_COPYRIGHT("Zaptel")
AC_REVISION($Revision: 2216 $)

ac_default_prefix=/usr
if test ${sysconfdir} = '${prefix}/etc'; then
   sysconfdir=/etc
fi
if test ${mandir} = '${prefix}/man'; then
   mandir=/usr/share/man
fi

if test ${localstatedir} = '${prefix}/var'; then
     localstatedir=/var
fi

# This needs to be before any macros that use the C compiler
AC_GNU_SOURCE

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AST_CHECK_GNU_MAKE

# Set the default value of HOSTCC from CC if --host was not provided:
HOSTCC=${HOSTCC:=${CC}}
AC_SUBST(HOSTCC)

AC_PATH_PROG([GREP], [grep], :)
AC_PATH_PROG([SHELL], [sh], :)
AC_PATH_PROG([LN], [ln], :)

AC_PATH_PROG([WGET], [wget], :)
if test "${WGET}" != ":" ; then
  DOWNLOAD=${WGET}
else
  AC_PATH_PROG([FETCH], [fetch], [:])
  DOWNLOAD=${FETCH}
fi
AC_SUBST(DOWNLOAD)

AC_LANG(C)

AST_EXT_LIB([curses], [initscr], [curses.h], [CURSES], [curses], [])
AST_EXT_LIB([ncurses], [initscr], [curses.h], [NCURSES], [ncurses], [])
AST_EXT_LIB([newt], [newtBell], [newt.h], [NEWT], [newt])
AST_EXT_LIB([usb], [usb_init], [usb.h], [USB], [libusb])

AC_CONFIG_FILES([build_tools/menuselect-deps makeopts])
AC_OUTPUT

AC_MSG_NOTICE(*** Zaptel build successfully configured ***)

