# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.60)

m4_define([PBX_VERSION],
          m4_bpatsubst(m4_esyscmd([build_tools/make_version .]),
                       [\([0-9.]*\)\(\w\|\W\)*],
                       [\1]))
AC_INIT(asterisk, PBX_VERSION, www.asterisk.org)

# cross-compile macros
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

# check existence of the package
AC_CONFIG_SRCDIR([main/asterisk.c])

# specify output header file
AC_CONFIG_HEADER(include/asterisk/autoconfig.h)

AC_COPYRIGHT("Asterisk")
AC_REVISION($Revision: 78166 $)

AC_GNU_SOURCE
AC_USE_SYSTEM_EXTENSIONS	# note- does not work on FreeBSD

case "${host_os}" in
     freebsd*)
     ac_default_prefix=/usr/local
     CPPFLAGS=-I/usr/local/include
     LDFLAGS=-L/usr/local/lib
     ;;
     *)
     ac_default_prefix=/usr
     if test ${sysconfdir} = '${prefix}/etc'; then
        sysconfdir=/etc
     fi
     if test ${mandir} = '${prefix}/man'; then
        mandir=/usr/share/man
     fi
     ;;
esac

if test ${localstatedir} = '${prefix}/var'; then
     localstatedir=/var
fi

BUILD_PLATFORM=${build}
BUILD_CPU=${build_cpu}
BUILD_VENDOR=${build_vendor}
BUILD_OS=${build_os}

AC_SUBST(BUILD_PLATFORM)
AC_SUBST(BUILD_CPU)
AC_SUBST(BUILD_VENDOR)
AC_SUBST(BUILD_OS)

HOST_PLATFORM=${host}
HOST_CPU=${host_cpu}
HOST_VENDOR=${host_vendor}
HOST_OS=${host_os}

AC_SUBST(HOST_PLATFORM)
AC_SUBST(HOST_CPU)
AC_SUBST(HOST_VENDOR)
AC_SUBST(HOST_OS)

case "${host_os}" in
     freebsd*)
     OSARCH=FreeBSD
     ;;
     netbsd*)
     OSARCH=NetBSD
     ;;
     openbsd*)
     OSARCH=OpenBSD
     ;;
     solaris*)
     OSARCH=SunOS
     ;;
     *)
     OSARCH=${HOST_OS}
     ;;
esac

AC_SUBST(OSARCH)

#  check for uname
AC_PATH_TOOL([UNAME], [uname], No)
if test ! x"${UNAME}" = xNo; then
   PBX_OSREV=$(${UNAME} -r)
fi
AC_SUBST(PBX_OSREV)

AH_TOP(
#ifndef ASTERISK_AUTOCONFIG_H
#define ASTERISK_AUTOCONFIG_H

#include "asterisk/buildopts.h"

)

AH_BOTTOM(
#endif
)

# cross-compile checks
if test "${cross_compiling}" = "yes"; 
then
   AC_CHECK_TOOL(CC, gcc, :)
   AC_CHECK_TOOL(CXX, g++, :)
   AC_CHECK_TOOL(LD, ld, :)
   AC_CHECK_TOOL(RANLIB, ranlib, :)
fi

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_CXXCPP
# This macro is just copied into our local acinclude.m4 from libtool.m4 so that
# the developers regenerating the configure script don't have to install libtool.
AST_PROG_LD 	# note - does not work on freebsd
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_RANLIB
AST_CHECK_GNU_MAKE

AC_PATH_TOOL([STRIP], [strip], :)
AC_PATH_TOOL([AR], [ar], :)

GNU_LD=0
if test "x$with_gnu_ld" = "xyes" ; then
   GNU_LD=1
fi
AC_SUBST(GNU_LD)

AC_PATH_PROG([GREP], [grep], :)
AC_PATH_PROG([FIND], [find], :)
AC_PATH_PROG([COMPRESS], [compress], :)
AC_PATH_PROG([BASENAME], [basename], :)
AC_PATH_PROG([ID], [id], :)
AC_PATH_PROG([DIRNAME], [dirname], :)
AC_PATH_PROG([SHELL], [sh], :)
AC_PATH_PROG([LN], [ln], :)
AC_PATH_PROG([DOT], [dot], :)
AC_PATH_PROG([WGET], [wget], :)
if test "${WGET}" != ":" ; then
  DOWNLOAD=${WGET}
else
  AC_PATH_PROG([FETCH], [fetch], [:])
  DOWNLOAD=${FETCH}
fi
AC_SUBST(DOWNLOAD)

ACX_PTHREAD

AC_LANG(C)

AC_ARG_ENABLE(dev-mode,
	[  --enable-dev-mode    Turn on developer mode],
	[case "${enableval}" in
	      y|ye|yes) AST_DEVMODE=yes ;;
	      n|no)  AST_DEVMODE=no ;;
	      *) AC_MSG_ERROR(bad value ${enableval} for --enable-dev-mode)  ;;
	esac])
AC_SUBST(AST_DEVMODE)

# package option names should be in alphabetical order
# by the --with option name, to make things easier for the users :-)

AST_EXT_LIB_SETUP([ALSA], [Advanced Linux Sound Architecture], [asound])
AST_EXT_LIB_SETUP([CURL], [cURL], [curl])
AST_EXT_LIB_SETUP([CURSES], [curses], [curses])
AST_EXT_LIB_SETUP([GNUTLS], [GNU TLS support (used for iksemel only)], [gnutls])
AST_EXT_LIB_SETUP([GSM], [GSM], [gsm], [, or 'internal'])
AST_EXT_LIB_SETUP([IKSEMEL], [Iksemel Jabber Library], [iksemel])
AST_EXT_LIB_SETUP([IMAP_TK], [UW IMAP Toolkit], [imap])
AST_EXT_LIB_SETUP([ISDNNET], [ISDN4Linux Library], [isdnnet])
AST_EXT_LIB_SETUP([KDE], [KDE], [kde])
AST_EXT_LIB_SETUP([LTDL], [libtool], [ltdl])
AST_EXT_LIB_SETUP([MISDN], [mISDN User Library], [misdn])
AST_EXT_LIB_SETUP([NBS], [Network Broadcast Sound], [nbs])
AST_EXT_LIB_SETUP([NCURSES], [ncurses], [ncurses])
AST_EXT_LIB_SETUP([NETSNMP], [Net-SNMP], [netsnmp])
AST_EXT_LIB_SETUP([NEWT], [newt], [newt])
AST_EXT_LIB_SETUP([UNIXODBC], [unixODBC], [odbc])
AST_EXT_LIB_SETUP([OGG], [OGG], [ogg])
AST_EXT_LIB_SETUP([OSPTK], [OSP Toolkit], [osptk])
AST_EXT_LIB_SETUP([OSS], [Open Sound System], [oss])
AST_EXT_LIB_SETUP([POPT], [popt], [popt])
AST_EXT_LIB_SETUP([PGSQL], [PostgreSQL], [postgres])
AST_EXT_LIB_SETUP([PRI], [ISDN PRI], [pri])
AST_EXT_LIB_SETUP([PWLIB], [PWlib], [pwlib])
AST_EXT_LIB_SETUP([OPENH323], [OpenH323], [h323])
AST_EXT_LIB_SETUP([QT], [Qt], [qt])
AST_EXT_LIB_SETUP([RADIUS], [Radius Client], [radius])
AST_EXT_LIB_SETUP([SPEEX], [Speex], [speex])
AST_EXT_LIB_SETUP([SQLITE], [SQLite], [sqlite])
AST_EXT_LIB_SETUP([SUPPSERV], [mISDN Supplemental Services], [suppserv])
AST_EXT_LIB_SETUP([OPENSSL], [OpenSSL], [ssl])
AST_EXT_LIB_SETUP([FREETDS], [FreeTDS], [tds])
AST_EXT_LIB_SETUP([TERMCAP], [Termcap], [termcap])
AST_EXT_LIB_SETUP([TINFO], [Term Info], [tinfo])
AST_EXT_LIB_SETUP([TONEZONE], [tonezone], [tonezone])
AST_EXT_LIB_SETUP([VORBIS], [Vorbis], [vorbis])
AST_EXT_LIB_SETUP([VPB], [Voicetronix API], [vpb])
AST_EXT_LIB_SETUP([ZLIB], [zlib], [z])
AST_EXT_LIB_SETUP([ZAPTEL], [Zaptel], [zaptel])

# check for basic system features and functionality before
# checking for package libraries

AC_FUNC_ALLOCA
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h fcntl.h inttypes.h libintl.h limits.h locale.h malloc.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h string.h strings.h sys/file.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h syslog.h termios.h unistd.h utime.h])

AC_SYS_LARGEFILE

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_UID_T
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_CHECK_MEMBERS([struct stat.st_blksize])
AC_HEADER_TIME
AC_STRUCT_TM
AC_C_VOLATILE
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_CHOWN
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_ERROR_AT_LINE
AST_FUNC_FORK
AC_FUNC_FSEEKO
AC_PROG_GCC_TRADITIONAL
# XXX: these are commented out until we determine whether it matters if our malloc()
# acts exactly like glibc's or not
# AC_FUNC_MALLOC
# AC_FUNC_REALLOC
AC_FUNC_MEMCMP
AC_FUNC_MKTIME
AC_FUNC_MMAP
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_SETVBUF_REVERSED
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_STRCOLL
AC_FUNC_STRFTIME
AC_FUNC_STRNLEN
AC_FUNC_STRTOD
AC_FUNC_UTIME_NULL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([asprintf atexit bzero dup2 endpwent floor ftruncate getcwd gethostbyname gethostname getloadavg gettimeofday inet_ntoa isascii localtime_r memchr memmove memset mkdir munmap pow putenv re_comp regcomp rint select setenv socket sqrt strcasecmp strcasestr strchr strcspn strdup strerror strlcat strlcpy strncasecmp strndup strnlen strrchr strsep strspn strstr strtol strtoq unsetenv utime vasprintf])

# some systems already have gethostbyname_r so we don't need to build ours in main/utils.c
AC_SEARCH_LIBS(gethostbyname_r, [socket nsl])

AC_MSG_CHECKING(for gethostbyname_r with 6 arguments)
AC_LINK_IFELSE(
        AC_LANG_PROGRAM([#include <stdlib.h>
                         #include <netdb.h>],
                        [struct hostent *he = gethostbyname_r((const char *)NULL, (struct hostent *)NULL, (char *)NULL, (int)0, (struct hostent **)NULL, (int *)NULL);]),
        AC_MSG_RESULT(yes)
        AC_DEFINE([HAVE_GETHOSTBYNAME_R_6], 1, [Define to 1 if your system has gethostbyname_r with 6 arguments.]),
        AC_MSG_RESULT(no)
)

AC_MSG_CHECKING(for gethostbyname_r with 5 arguments)
AC_LINK_IFELSE(
        AC_LANG_PROGRAM([#include <stdlib.h>
	                 #include <netdb.h>],
                        [struct hostent *he = gethostbyname_r((const char *)NULL, (struct hostent *)NULL, (char *)NULL, (int)0, (int *)NULL);]),
        AC_MSG_RESULT(yes)
        AC_DEFINE([HAVE_GETHOSTBYNAME_R_5], 1, [Define to 1 if your system has gethostbyname_r with 5 arguments.]),
        AC_MSG_RESULT(no)
)

AC_MSG_CHECKING(for PTHREAD_RWLOCK_INITIALIZER)
AC_LINK_IFELSE(
        AC_LANG_PROGRAM([#include <pthread.h>],
                        [int foo = PTHREAD_RWLOCK_INITIALIZER;]),
        AC_MSG_RESULT(yes)
        AC_DEFINE([HAVE_PTHREAD_RWLOCK_INITIALIZER], 1, [Define to 1 if your system has PTHREAD_RWLOCK_INITIALIZER.]),
        AC_MSG_RESULT(no)
)

AC_MSG_CHECKING(for PTHREAD_RWLOCK_PREFER_WRITER_NP)
AC_LINK_IFELSE(
        AC_LANG_PROGRAM([#include <pthread.h>],
                        [int foo = PTHREAD_RWLOCK_PREFER_WRITER_NP;]),
        AC_MSG_RESULT(yes)
        AC_DEFINE([HAVE_PTHREAD_RWLOCK_PREFER_WRITER_NP], 1, [Define to 1 if your system has PTHREAD_RWLOCK_PREFER_WRITER_NP.]),
        AC_MSG_RESULT(no)
)

AC_MSG_CHECKING(for compiler atomic operations)
AC_LINK_IFELSE(
AC_LANG_PROGRAM([], [int foo1; int foo2 = __sync_fetch_and_add(&foo1, 1);]),
AC_MSG_RESULT(yes)
AC_DEFINE([HAVE_GCC_ATOMICS], 1, [Define to 1 if your GCC C compiler provides atomic operations.]),
AC_MSG_RESULT(no)
)

AST_GCC_ATTRIBUTE(pure)
AST_GCC_ATTRIBUTE(malloc)
AST_GCC_ATTRIBUTE(const)
AST_GCC_ATTRIBUTE(unused)
AST_GCC_ATTRIBUTE(always_inline)

AC_MSG_CHECKING(for -ffunction-sections support)
saved_CFLAGS="${CFLAGS}"
CFLAGS="${CFLAGS} -ffunction-sections"
AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([], [int x = 1;]),
	AC_MSG_RESULT(yes)
	[saved_LDFLAGS="${LDFLAGS}"]
	[LDFLAGS="${LDFLAGS} -Wl,--gc-sections"]
	AC_MSG_CHECKING(for --gc-sections support)
	AC_LINK_IFELSE(
		AC_LANG_PROGRAM([], [int x = 1;]),
		AC_MSG_RESULT(yes)
		[GC_CFLAGS="-ffunction-sections"]
		[[GC_LDFLAGS="-Wl,--gc-sections"]],
		AC_MSG_RESULT(no)
	)
	[LDFLAGS="${saved_LDFLAGS}"],
	AC_MSG_RESULT(no)
)
CFLAGS="${saved_CFLAGS}"
AC_SUBST(GC_CFLAGS)
AC_SUBST(GC_LDFLAGS)

AC_MSG_CHECKING(for -Wdeclaration-after-statement support)
if $(${CC} -Wdeclaration-after-statement -S -o /dev/null -xc /dev/null > /dev/null 2>&1); then
   AC_MSG_RESULT(yes)
   AST_DECLARATION_AFTER_STATEMENT=-Wdeclaration-after-statement
else
	AC_MSG_RESULT(no)
	AST_DECLARATION_AFTER_STATEMENT=
fi
AC_SUBST(AST_DECLARATION_AFTER_STATEMENT)

AC_MSG_CHECKING(for res_ninit)
AC_LINK_IFELSE(
	AC_LANG_PROGRAM([#include <resolv.h>],
			[int foo = res_ninit(NULL);]),
	AC_MSG_RESULT(yes)
	AC_DEFINE([HAVE_RES_NINIT], 1, [Define to 1 if your system has the re-entrant resolver functions.])
	AC_MSG_CHECKING(for res_ndestroy)
	AC_LINK_IFELSE(
		AC_LANG_PROGRAM([#include <resolv.h>],
				[int foo = res_ndestroy(NULL);]),
		AC_MSG_RESULT(yes)
		AC_DEFINE([HAVE_RES_NDESTROY], 1, [Define to 1 if your system has the ndestroy resolver function.]),
		AC_MSG_RESULT(no)
	),
	AC_MSG_RESULT(no)
)

AC_MSG_CHECKING(for RTLD_NOLOAD)
AC_LINK_IFELSE(
	AC_LANG_PROGRAM([#include <dlfcn.h>],
			[int foo = RTLD_NOLOAD;]),
	AC_MSG_RESULT(yes)
	AC_DEFINE([HAVE_RTLD_NOLOAD], 1, [Define to 1 if your system has a dynamic linker that supports RTLD_NOLOAD.]),
	AC_MSG_RESULT(no)
)

AC_MSG_CHECKING(for IP_MTU_DISCOVER)
AC_LINK_IFELSE(
	AC_LANG_PROGRAM([#include <netinet/in.h>],
			[int foo = IP_MTU_DISCOVER;]),
	AC_MSG_RESULT(yes)
	AC_DEFINE([HAVE_IP_MTU_DISCOVER], 1, [Define to 1 if your system has PMTU discovery on UDP sockets.]),
	AC_MSG_RESULT(no)
)

AC_CHECK_HEADER([libkern/OSAtomic.h],
                [AC_DEFINE_UNQUOTED([HAVE_OSX_ATOMICS], 1, [Define to 1 if OSX atomic operations are supported.])])

AC_CHECK_SIZEOF(int)

# do the package library checks now

AST_EXT_LIB_CHECK([ALSA], [asound], [snd_spcm_init], [alsa/asoundlib.h], [-lm -ldl])

AST_EXT_LIB_CHECK([CURSES], [curses], [initscr], [curses.h])

GSM_INTERNAL="yes"
AC_SUBST(GSM_INTERNAL)
GSM_SYSTEM="yes"
if test "${USE_GSM}" != "no"; then
   if test "${GSM_DIR}" = "internal"; then
      GSM_SYSTEM="no"
   elif test "${GSM_DIR}" != ""; then
      GSM_INTERNAL="no"
   fi
   if test "${GSM_SYSTEM}" = "yes"; then
      gsmlibdir=""
      if test "x${GSM_DIR}" != "x"; then
         if test -d ${GSM_DIR}/lib; then
            gsmlibdir="-L${GSM_DIR}/lib"
         else
            gsmlibdir="-L${GSM_DIR}"
         fi
      fi
      AC_CHECK_LIB([gsm], [gsm_create], AC_DEFINE_UNQUOTED([HAVE_GSM], 1,
      [Define to indicate the GSM library]), [], ${gsmlibdir})
      if test "${ac_cv_lib_gsm_gsm_create}" = "yes"; then
         if test "x${GSM_DIR}" != "x" ; then
            AC_CHECK_HEADER([${GSM_DIR}/include/gsm.h], [GSM_HEADER_FOUND=1], [GSM_HEADER_FOUND=0])
            AC_CHECK_HEADER([${GSM_DIR}/include/gsm/gsm.h], [GSM_GSM_HEADER_FOUND=1], [GSM_GSM_HEADER_FOUND=0])
         else
            AC_CHECK_HEADER([gsm.h], [GSM_HEADER_FOUND=1], [GSM_HEADER_FOUND=0])
            AC_CHECK_HEADER([gsm/gsm.h], [GSM_GSM_HEADER_FOUND=1], [GSM_GSM_HEADER_FOUND=0])
         fi
         if test "${GSM_HEADER_FOUND}" = "0" ; then
            if test "{GSM_GSM_HEADER_FOUND}" = "0" ; then
               if test "x${GSM_MANDATORY}" = "xyes" ; then
                  AC_MSG_NOTICE([***])
                  AC_MSG_NOTICE([*** It appears that you do not have the gsm development package installed.])
                  AC_MSG_NOTICE([*** Please install it to include ${GSM_DESCRIP} support, or re-run configure])
                  AC_MSG_NOTICE([*** without explicitly specifying --with-${GSM_OPTION}])
                  exit 1
               fi
            fi
         fi
         GSM_OK=0
         if test "${GSM_HEADER_FOUND}" = "1" ; then
            AC_DEFINE_UNQUOTED([HAVE_GSM_HEADER], 1, [Define to indicate that gsm.h has no prefix for its location])
            GSM_OK=1
         else
            if test "${GSM_GSM_HEADER_FOUND}" = "1" ; then
               AC_DEFINE_UNQUOTED([HAVE_GSM_GSM_HEADER], 1, [Define to indicate that gsm.h is in gsm/gsm.h])
               GSM_OK=1
            fi
         fi
         if test "${GSM_OK}" = "1" ; then
            GSM_LIB="-lgsm"
            if test "x${GSM_DIR}" != "x"; then
               GSM_LIB="${gsmlibdir} ${GSM_LIB}"
               GSM_INCLUDE="-I${GSM_DIR}/include"
            fi
            PBX_GSM=1
            GSM_INTERNAL="no"
         fi
      fi
   fi
   if test "${GSM_INTERNAL}" = "yes"; then
      PBX_GSM=1
      AC_DEFINE_UNQUOTED([HAVE_GSM_HEADER], 1, [Define to indicate that gsm.h has no prefix for its location])
   fi
fi

AST_EXT_LIB_CHECK([IKSEMEL], [iksemel], [iks_start_sasl], [iksemel.h])

if test "${PBX_IKSEMEL}" = 1; then
   AST_EXT_LIB_CHECK([GNUTLS], [gnutls], [gnutls_bye], [gnutls/gnutls.h], [-lz -lgcrypt -lgpg-error])
fi

if test "${USE_IMAP_TK}" != "no"; then
   if test "${IMAP_TK_DIR}" = ""; then
      IMAP_TK_DIR=`pwd`"/../imap-2004g"
      if test  -n "${IMAP_TK_MANDATORY}"; then
	 AC_MSG_NOTICE([The --with-imap option does not search your system for installed])
	 AC_MSG_NOTICE([c-client library/header files. Since you did not provide a path])
	 AC_MSG_NOTICE([the configure script will assume you have placed built the c-client])
	 AC_MSG_NOTICE([files at ${IMAP_TK_DIR}, as outlined in the doc/imapstorage.txt file.])
      fi
   fi
   AC_MSG_CHECKING(for UW IMAP Toolkit c-client library)
   saved_cppflags="${CPPFLAGS}"
   saved_libs="${LIBS}"
   if test -f ${IMAP_TK_DIR}/c-client/LDFLAGS ; then
      imap_ldflags=`cat ${IMAP_TK_DIR}/c-client/LDFLAGS`
   fi
   CPPFLAGS="${CPPFLAGS} -I${IMAP_TK_DIR}/c-client"
   LIBS="${LIBS} ${IMAP_TK_DIR}/c-client/c-client.a "`echo ${imap_ldflags}`
   AC_LINK_IFELSE(
	AC_LANG_PROGRAM(
		[#include "c-client.h"
		void mm_searched (MAILSTREAM *stream,unsigned long number)
		{
		}
		void mm_exists (MAILSTREAM *stream,unsigned long number)
		{
		}
		void mm_expunged (MAILSTREAM *stream,unsigned long number)
		{
		}
		void mm_flags (MAILSTREAM *stream,unsigned long number)
		{
		}
		void mm_notify (MAILSTREAM *stream,char *string,long errflg)
		{
		}
		void mm_list (MAILSTREAM *stream,int delimiter,char *mailbox,long attributes)
		{
		}
		void mm_lsub (MAILSTREAM *stream,int delimiter,char *mailbox,long attributes)
		{
		}
		void mm_status (MAILSTREAM *stream,char *mailbox,MAILSTATUS *status)
		{
		}
		void mm_log (char *string,long errflg)
		{
		}
		void mm_dlog (char *string)
		{
		}
		void mm_login (NETMBX *mb,char *user,char *pwd,long trial)
		{
		}
		void mm_critical (MAILSTREAM *stream)
		{
		}
		void mm_nocritical (MAILSTREAM *stream)
		{
		}
		long mm_diskerror (MAILSTREAM *stream,long errcode,long serious)
		{
		}
		void mm_fatal (char *string)
		{
		}],
		[
		MAILSTREAM *foo = mail_open(NULL, "", 0);
		]
	),
	[ac_cv_imap_tk="yes"],
	[ac_cv_imap_tk="no"]
   )
   if test "${ac_cv_imap_tk}" = "yes"; then
   AC_LINK_IFELSE(
      AC_LANG_PROGRAM(
         [#include "c-client.h"
         void mm_searched (MAILSTREAM *stream,unsigned long number)
         {
         }
         void mm_exists (MAILSTREAM *stream,unsigned long number)
         {
         }
         void mm_expunged (MAILSTREAM *stream,unsigned long number)
         {
         }
         void mm_flags (MAILSTREAM *stream,unsigned long number)
         {
         }
         void mm_notify (MAILSTREAM *stream,char *string,long errflg)
         {
         }
         void mm_list (MAILSTREAM *stream,int delimiter,char *mailbox,long attributes)
         {
         }
         void mm_lsub (MAILSTREAM *stream,int delimiter,char *mailbox,long attributes)
         {
         }
         void mm_status (MAILSTREAM *stream,char *mailbox,MAILSTATUS *status)
         {
         }
         void mm_log (char *string,long errflg)
         {
         }
         void mm_dlog (char *string)
         {
         }
         void mm_login (NETMBX *mb,char *user,char *pwd,long trial)
         {
         }
         void mm_critical (MAILSTREAM *stream)
         {
         }
         void mm_nocritical (MAILSTREAM *stream)
         {
         }
         long mm_diskerror (MAILSTREAM *stream,long errcode,long serious)
         {
         }
         void mm_fatal (char *string)
         {
         }],
         [
         long check = mail_expunge_full(NULL, "", 0);
         ]
      ),
      [ac_cv_imap_tk2006="yes"],
      [ac_cv_imap_tk2006="no"]
  )
  fi
   CPPFLAGS="${saved_cppflags}"
   LIBS="${saved_libs}"
   if test "${ac_cv_imap_tk}" = "yes"; then
      AC_MSG_RESULT(yes) 
      IMAP_TK_LIB="${IMAP_TK_DIR}/c-client/c-client.a "`echo ${imap_ldflags}`
      IMAP_TK_INCLUDE="-I${IMAP_TK_DIR}/c-client"
      PBX_IMAP_TK=1
      AC_DEFINE([HAVE_IMAP_TK], 1, [Define if your system has the UW IMAP Toolkit c-client library.])
      if test "${ac_cv_imap_tk2006}" = "yes"; then
         AC_DEFINE([HAVE_IMAP_TK2006], 1, [Define if your system has the UW IMAP Toolkit c-client library version 2006 or greater.])
      fi
   elif test -n "${IMAP_TK_MANDATORY}"; then
      AC_MSG_RESULT(no) 
      AC_MSG_NOTICE([***])
      AC_MSG_NOTICE([*** The UW IMAP Toolkit installation on this system appears to be broken.])
      AC_MSG_NOTICE([*** Either correct the installation, or run configure])
      AC_MSG_NOTICE([*** including --without-imap.])
      exit 1
   else
      AC_MSG_RESULT(no) 
   fi
fi

# Needed by unixodbc
AST_EXT_LIB_CHECK([LTDL], [ltdl], [lt_dlinit], [ltdl.h], [])

AC_LANG_PUSH(C++)

if test "${USE_KDE}" != "no"; then
   AC_MSG_CHECKING(for crashHandler in -lkdecore)
   saved_libs="${LIBS}"
   saved_cppflags="${CPPFLAGS}"
   CPPFLAGS="${CPPFLAGS} -I${KDE_DIR}/include"
   if test -d ${KDE_DIR}/lib; then
      kdelibdir="${KDE_DIR}/lib"
   else
      kdelibdir="${KDE_DIR}"
   fi
   LIBS="${LIBS} -L${kdelibdir} -lkdecore"

   AC_LINK_IFELSE(
	[AC_LANG_PROGRAM(
			[#include "kcrash.h"],
			[KCrash::defaultCrashHandler(1);])
	],
	[ac_cv_lib_kde_crash="yes"],
	[ac_cv_lib_kde_crash="no"])
		
   LIBS="${saved_libs}"
   CPPFLAGS="${saved_cppflags}"
	
   if test "${ac_cv_lib_kde_crash}" = "yes"; then
      AC_MSG_RESULT(yes) 
      KDE_LIB="-lkdecore -lkdeui"
      if test "${KDE_DIR}" != ""; then
      	 KDE_LIB="-L${kdelibdir} ${KDE_LIB}"
	 KDE_INCLUDE="-I${KDE_DIR}/include"
      fi
      PBX_KDE=1
      AC_DEFINE([HAVE_LIBKDE], 1, [Define if your system has the KDE libraries.])
   elif test -n "${KDE_MANDATORY}"; then
      AC_MSG_RESULT(no) 
      AC_MSG_NOTICE([***])
      AC_MSG_NOTICE([*** The KDE installation on this system appears to be broken.])
      AC_MSG_NOTICE([*** Either correct the installation, or run configure])
      AC_MSG_NOTICE([*** including --without-kde.])
      exit 1
   else
      AC_MSG_RESULT(no) 
   fi
fi
if test "${PBX_KDE}" = 1; then
   AC_PATH_TOOL(KDEINIT, kdeinit, No)
   if test ! x"${KDEINIT}" = xNo; then
      KDEDIR=$(${DIRNAME} ${KDEINIT})
      KDEDIR=$(${DIRNAME} ${KDEDIR})
   fi
   AC_SUBST([KDEDIR])
fi

AC_LANG_POP

AST_EXT_LIB_CHECK([MISDN], [mISDN], [mISDN_open], [mISDNuser/mISDNlib.h])

if test "${PBX_MISDN}" = 1; then
   AST_EXT_LIB_CHECK([ISDNNET], [isdnnet], [init_manager], [mISDNuser/isdn_net.h], [-lmISDN -lpthread])
   AST_EXT_LIB_CHECK([SUPPSERV], [suppserv], [encodeFac], [mISDNuser/suppserv.h])
   AC_CHECK_HEADER([linux/mISDNdsp.h], [AC_DEFINE_UNQUOTED([MISDN_1_2], 1, [Build chan_misdn for mISDN 1.2 or later.])])
fi

AST_EXT_LIB_CHECK([NBS], [nbs], [nbs_connect], [nbs.h])

AST_EXT_LIB_CHECK([NCURSES], [ncurses], [initscr], [curses.h])

NETSNMP_CONFIG=No
if test "${USE_NETSNMP}" != "no"; then	
   if test "x${NETSNMP_DIR}" != "x"; then
      AC_PATH_TOOL([NETSNMP_CONFIG], [net-snmp-config], No, [${NETSNMP_DIR}/bin])
      if test x"${NETSNMP_CONFIG}" = xNo; then
         AC_MSG_NOTICE([***])
         AC_MSG_NOTICE([*** net-snmp-config was not found in the path you specified:])
         AC_MSG_NOTICE([*** ${NETSNMP_DIR}/bin])
         AC_MSG_NOTICE([*** Either correct the installation, or run configure])
         AC_MSG_NOTICE([*** including --without-netsnmp])
         exit 1
      fi
   else
      AC_PATH_TOOL([NETSNMP_CONFIG], [net-snmp-config], No)
   fi
fi
if test x"${NETSNMP_CONFIG}" != xNo; then
   NETSNMP_libs=`${NETSNMP_CONFIG} --agent-libs`
   
   AC_CHECK_LIB([netsnmp], [snmp_register_callback], AC_DEFINE_UNQUOTED([HAVE_NETSNMP], 1,
   [Define to indicate the Net-SNMP library]), [], ${NETSNMP_libs})

   if test "${ac_cv_lib_netsnmp_snmp_register_callback}" = "yes"; then
      NETSNMP_LIB="${NETSNMP_libs}"
      PBX_NETSNMP=1
   elif test -n "${NETSNMP_MANDATORY}";
   then
      AC_MSG_NOTICE([***])
      AC_MSG_NOTICE([*** The Net-SNMP installation on this system appears to be broken.])
      AC_MSG_NOTICE([*** Either correct the installation, or run configure])
      AC_MSG_NOTICE([*** including --without-netsnmp])
      exit 1
   fi
elif test -n "${NETSNMP_MANDATORY}";
then
   AC_MSG_NOTICE([***])
   AC_MSG_NOTICE([*** The Net-SNMP installation on this system appears to be broken.])
   AC_MSG_NOTICE([*** Either correct the installation, or run configure])
   AC_MSG_NOTICE([*** including --without-netsnmp])
   exit 1
fi

AST_EXT_LIB_CHECK([NEWT], [newt], [newtBell], [newt.h])

AST_EXT_LIB_CHECK([UNIXODBC], [odbc], [SQLConnect], [sql.h], [])

AST_EXT_LIB_CHECK([OGG], [ogg], [ogg_sync_init], [])

if test "${USE_OSS}" != "no"; then
PBX_OSS=0
AC_CHECK_HEADER([linux/soundcard.h],
	        [
		PBX_OSS=1
                AC_DEFINE_UNQUOTED([HAVE_OSS], 1, [Define to indicate the Open Sound System library])
		])
if test "$PBX_OSS" = "0"; then
   AC_CHECK_HEADER([sys/soundcard.h],
                   [
		   PBX_OSS=1
		   AC_DEFINE_UNQUOTED([HAVE_OSS], 1, [Define to indicate the Open Sound System library])
		   ])
fi
if test "$PBX_OSS" = "0"; then
   AST_EXT_LIB_CHECK([OSS], [ossaudio], [oss_ioctl_mixer], [soundcard.h])
fi
fi

PG_CONFIG=No
if test "${USE_PGSQL}" != "no"; then	
   if test "x${PGSQL_DIR}" != "x"; then
      AC_PATH_TOOL([PG_CONFIG], [pg_config], No, [${PGSQL_DIR}/bin])
      if test x"${PG_CONFIG}" = xNo; then
         AC_MSG_NOTICE([***])
         AC_MSG_NOTICE([*** pg_config was not found in the path you specified:])
         AC_MSG_NOTICE([*** ${PGSQL_DIR}/bin])
         AC_MSG_NOTICE([*** Either correct the installation, or run configure])
         AC_MSG_NOTICE([*** including --without-postgres])
         exit 1
      fi
   else
      AC_PATH_TOOL([PG_CONFIG], [pg_config], No)
   fi
fi
if test "${PG_CONFIG}" != No; then
   PGSQL_libdir=`${PG_CONFIG} --libdir`
   PGSQL_includedir=`${PG_CONFIG} --includedir`

   if test "x$?" != "x0" ; then
      if test -n "${PGSQL_MANDATORY}" ; then
         AC_MSG_NOTICE([***])
         AC_MSG_NOTICE([*** The PostgreSQL installation on this system appears to be broken.])
         AC_MSG_NOTICE([*** Either correct the installation, or run configure])
         AC_MSG_NOTICE([*** including --without-postgres])
         exit 1
	  fi
   else 
      AC_CHECK_LIB([pq], [PQexec], AC_DEFINE_UNQUOTED([HAVE_PGSQL], 1,
      [Define to indicate the PostgreSQL library]), [], -L${PGSQL_libdir} -lz)

      if test "${ac_cv_lib_pq_PQexec}" = "yes"; then
         PGSQL_LIB="-L${PGSQL_libdir} -lpq -lz"
         PGSQL_INCLUDE="-I${PGSQL_includedir}"
         PBX_PGSQL=1
      elif test -n "${PGSQL_MANDATORY}";
      then
         AC_MSG_NOTICE([***])
         AC_MSG_NOTICE([*** The PostgreSQL installation on this system appears to be broken.])
         AC_MSG_NOTICE([*** Either correct the installation, or run configure])
         AC_MSG_NOTICE([*** including --without-postgres])
         exit 1
      fi
   fi
elif test -n "${PGSQL_MANDATORY}";
then
   AC_MSG_NOTICE([***])
   AC_MSG_NOTICE([*** The PostgreSQL installation on this system appears to be broken.])
   AC_MSG_NOTICE([*** Either correct the installation, or run configure])
   AC_MSG_NOTICE([*** including --without-postgres])
   exit 1
fi

AST_EXT_LIB_CHECK([POPT], [popt], [poptStrerror], [popt.h])

AST_EXT_LIB_CHECK([PRI], [pri], [pri_keypad_facility], [libpri.h])

if test "${USE_PWLIB}" != "no"; then
	if test -n "${PWLIB_DIR}"; then
		PWLIBDIR="${PWLIB_DIR}"
	fi
	AST_CHECK_PWLIB()
	AST_CHECK_PWLIB_VERSION([PWLib], [PWLIB], [ptbuildopts.h], [1], [9], [2])
		
	if test "${HAS_PWLIB:-unset}" != "unset"; then
		AST_CHECK_OPENH323_PLATFORM()

		PLATFORM_PWLIB="pt_${PWLIB_PLATFORM}_r"

		AST_CHECK_PWLIB_BUILD([PWLib], [PWLIB],
			[Define if your system has the PWLib libraries.],
			[#include "ptlib.h"],
			[BOOL q = PTime::IsDaylightSavings();])
	fi
fi

if test "${USE_PWLIB}" != "no" -a "x${ac_cv_lib_PWLIB}" != "xyes" -a -n "${PWLIB_MANDATORY}"; then
   AC_MSG_NOTICE([***])
   AC_MSG_NOTICE([*** The PWLIB installation on this system appears to be broken.])
   AC_MSG_NOTICE([*** Either correct the installation, or run configure])
   AC_MSG_NOTICE([*** including --without-pwlib])
   exit 1
fi

if test "${PBX_PWLIB}" = "1" -a "${USE_OPENH323}" != "no" ; then
	if test -n "${OPENH323_DIR}"; then
		OPENH323DIR="${OPENH323_DIR}"
	fi
	AST_CHECK_OPENH323()
	AST_CHECK_PWLIB_VERSION([OpenH323], [OPENH323], [openh323buildopts.h], [1], [17], [3])
	AST_CHECK_OPENH323_BUILD()
	PLATFORM_OPENH323="h323_${PWLIB_PLATFORM}_${OPENH323_SUFFIX}"
	AST_CHECK_PWLIB_BUILD([OpenH323], [OPENH323],
		[Define if your system has the OpenH323 libraries.],
		[#include "ptlib.h"
		#include "h323.h"
		#include "h323ep.h"],
		[H323EndPoint ep = H323EndPoint();],
		[${PWLIB_INCLUDE}], [${PWLIB_LIB}])
fi
if test "${USE_OPENH323}" != "no" -a "x${ac_cv_lib_OPENH323}" != "xyes" -a -n "${OPENH323_MANDATORY}"; then
   AC_MSG_NOTICE([***])
   AC_MSG_NOTICE([*** The OPENH323 installation on this system appears to be broken.])
   AC_MSG_NOTICE([*** Either correct the installation, or run configure])
   AC_MSG_NOTICE([*** including --without-h323])
   exit 1
fi


AC_LANG_PUSH(C++)

if test "${USE_QT}" != "no"; then
   AC_MSG_CHECKING(for QDate in -lqt)
   saved_libs="${LIBS}"
   saved_cppflags="${CPPFLAGS}"
   if test "x${QT_DIR}" != "x"; then
      LIBS="${LIBS} -L${QT_DIR}/lib"
      CPPFLAGS="${CPPFLAGS} -I${QT_DIR}/include"
   fi
   LIBS="${LIBS} -lqt"
   qtlib="qt"
   AC_LINK_IFELSE(
	[
		AC_LANG_PROGRAM(
		[#include <qt3/qdatetime.h>],
		[QDate date();])
	],
	[ac_cv_lib_qt_qt_date="yes"],
	[ac_cv_lib_qt_qt_date="no"])
   LIBS="${saved_libs}"
   CPPFLAGS="${saved_cppflags}"
	
   if test "${ac_cv_lib_qt_qt_date}" = "no"; then
      saved_libs="${LIBS}"
      saved_cppflags="${CPPFLAGS}"
      if test "x${QT_DIR}" != "x"; then
      	 LIBS="${LIBS} -L${QT_DIR}/lib"
	 CPPFLAGS="${CPPFLAGS} -I${QT_DIR}/include"
      fi
      LIBS="${LIBS} -lqt-mt"
      qtlib="qt-mt"	
      AC_LINK_IFELSE(
		[
			AC_LANG_PROGRAM(
			[#include <qt3/qdatetime.h>],
			[QDate date();])
		],
		[ac_cv_lib_qt_qt_date="yes"],
		[ac_cv_lib_qt_qt_date="no"])
      LIBS="${saved_libs}"
      CPPFLAGS="${saved_cppflags}"
   fi	

   if test "${ac_cv_lib_qt_qt_date}" = "yes"; then
      AC_MSG_RESULT(yes) 
   else
      AC_MSG_RESULT(no) 
   fi
	
   if test "${ac_cv_lib_qt_qt_date}" = "yes"; then
      QT_LIB="-l${qtlib}"
      if test "${QT_DIR}" != ""; then
      	 QT_LIB="-L${QT_DIR}/lib ${QT_LIB}"
	 QT_INCLUDE="-I${QT_DIR}/include"
      fi
      PBX_QT=1
      AC_DEFINE([HAVE_QT], 1, [Define if your system has the Qt library])
      AC_PATH_TOOL(QTMOC, moc, No)
   elif test -n "${QT_MANDATORY}"; 
   then
      AC_MSG_NOTICE([***])
      AC_MSG_NOTICE([*** The Qt installation on this system appears to be broken.])
      AC_MSG_NOTICE([*** Either correct the installation, or run configure])
      AC_MSG_NOTICE([*** including --without-qt.])
      exit 1
   fi
fi

AC_LANG_POP

AST_EXT_LIB_CHECK([RADIUS], [radiusclient-ng], [rc_read_config], [radiusclient-ng.h])

AST_EXT_LIB_CHECK([SPEEX], [speex], [speex_encode], [speex/speex.h], [-lm])

AST_EXT_LIB_CHECK([SQLITE], [sqlite], [sqlite_exec], [sqlite.h])

AST_EXT_LIB_CHECK([OPENSSL], [ssl], [ssl2_connect], [openssl/ssl.h], [-lcrypto])
if test "$PBX_OPENSSL" = "1";
then
    AST_EXT_LIB_CHECK([OSPTK], [osptk], [OSPPCryptoDecrypt], [osp/osp.h], [-lcrypto -lssl])
fi

AST_EXT_LIB_CHECK([FREETDS], [tds], [tds_version], [tds.h])
if test "${PBX_FREETDS}" != "0";
then
    case `grep TDS_VERSION_NO ${FREETDS_DIR:-/usr}/include/tdsver.h` in
	*0.64*)
		FREETDS_INCLUDE="${FREETDS_INCLUDE} -DFREETDS_0_64"
	;;
    *0.63*)
        FREETDS_INCLUDE="${FREETDS_INCLUDE} -DFREETDS_0_63"
	;;
    *0.62*)
        FREETDS_INCLUDE="${FREETDS_INCLUDE} -DFREETDS_0_62"
	;;
    *)
        FREETDS_INCLUDE="${FREETDS_INCLUDE} -DFREETDS_PRE_0_62"
	;;
    esac
fi

AST_EXT_LIB_CHECK([TERMCAP], [termcap], [tgetent], [])

AST_EXT_LIB_CHECK([TINFO], [tinfo], [tgetent], [])

if test "${host_os}" != "linux-gnu" ; then
  tonezone_extra="-lm"
fi

AST_EXT_LIB_CHECK([TONEZONE], [tonezone], [tone_zone_find], [zaptel/tonezone.h], [${tonezone_extra}])

AST_EXT_LIB_CHECK([VORBIS], [vorbis], [vorbis_info_init], [vorbis/codec.h], [-lm -lvorbisenc])

AC_LANG_PUSH(C++)

if test "${USE_VPB}" != "no"; then
   AC_MSG_CHECKING(for vpb_open in -lvpb)
   saved_libs="${LIBS}"
   saved_cppflags="${CPPFLAGS}"
   if test "x${VPB_DIR}" != "x"; then
      if test -d ${VPB_DIR}/lib; then
         vpblibdir=${VPB_DIR}/lib
      else
         vpblibdir=${VPB_DIR}
      fi
      LIBS="${LIBS} -L${vpblibdir}"
      CPPFLAGS="${CPPFLAGS} -I${VPB_DIR}/include"
   fi
   LIBS="${LIBS} -lvpb -lpthread"
   AC_LINK_IFELSE(
	[
	AC_LANG_PROGRAM(
	[#include <vpbapi.h>],
	[int q = vpb_open(0,0);])
	],
	[	AC_MSG_RESULT(yes) 
		ac_cv_lib_vpb_vpb_open="yes" 
	],
	[	AC_MSG_RESULT(no) 
		ac_cv_lib_vpb_vpb_open="no" 
	]
	)
   LIBS="${saved_libs}"
   CPPFLAGS="${saved_cppflags}"
   if test "${ac_cv_lib_vpb_vpb_open}" = "yes"; then
	VPB_LIB="-lvpb"
	if test "${VPB_DIR}" != ""; then
	   VPB_LIB="-L${vpblibdir}  ${VPB_LIB}"
	   VPB_INCLUDE="-I${VPB_DIR}/include"
	fi
	PBX_VPB=1
	AC_DEFINE([HAVE_VPB], 1, [Define if your system has the VoiceTronix API libraries.])
   elif test -n "${VPB_MANDATORY}"; then
      AC_MSG_NOTICE([***])
      AC_MSG_NOTICE([*** The VoiceTronix (vpb) installation on this system appears to be broken.])
      AC_MSG_NOTICE([*** Either correct the installation, or run configure])
      AC_MSG_NOTICE([*** including --without-vpb.])
      exit 1
   fi
fi

AC_LANG_POP

AST_EXT_LIB_CHECK([ZLIB], [z], [compress], [zlib.h])

if test "${USE_ZAPTEL}" != "no"; then
   AC_MSG_CHECKING(for ZT_DIAL_OP_CANCEL in zaptel/zaptel.h)
   saved_cppflags="${CPPFLAGS}"
   if test "x${ZAPTEL_DIR}" != "x"; then
      CPPFLAGS="${CPPFLAGS} -I${ZAPTEL_DIR}/include"
   fi
   AC_COMPILE_IFELSE(
	[
	AC_LANG_PROGRAM(
	[#include <zaptel/zaptel.h>],
	[int foo = ZT_DIAL_OP_CANCEL;])
	],
	[	AC_MSG_RESULT(yes) 
		ac_cv_zaptel_h="yes" 
	],
	[	AC_MSG_RESULT(no) 
		ac_cv_zaptel_h="no" 
	]
	)
   CPPFLAGS="${saved_cppflags}"
   if test "${ac_cv_zaptel_h}" = "yes"; then
	if test "${ZAPTEL_DIR}" != ""; then
	   ZAPTEL_INCLUDE="-I${ZAPTEL_DIR}/include"
	fi
	PBX_ZAPTEL=1
	AC_DEFINE([HAVE_ZAPTEL], 1, [Define if your system has the Zaptel headers.])
   elif test -n "${ZAPTEL_MANDATORY}"; 
   then
      AC_MSG_NOTICE([***])
      AC_MSG_NOTICE([*** The Zaptel installation on this system appears to be broken.])
      AC_MSG_NOTICE([*** Either correct the installation, or run configure])
      AC_MSG_NOTICE([*** including --without-zaptel.])
      exit 1
   fi
fi

if test "${PBX_ZAPTEL}" = 1; then
   AC_MSG_CHECKING(for ZT_EVENT_REMOVED in zaptel/zaptel.h)
   saved_cppflags="${CPPFLAGS}"
   if test "x${ZAPTEL_DIR}" != "x"; then
      CPPFLAGS="${CPPFLAGS} -I${ZAPTEL_DIR}/include"
   fi
   AC_COMPILE_IFELSE(
	[
	AC_LANG_PROGRAM(
	[#include <zaptel/zaptel.h>],
	[int foo = ZT_EVENT_REMOVED;])
	],
	[	AC_MSG_RESULT(yes) 
		ac_cv_zaptel_vldtmf="yes" 
	],
	[	AC_MSG_RESULT(no) 
		ac_cv_zaptel_vldtmf="no" 
	]
	)
   CPPFLAGS="${saved_cppflags}"
   if test "${ac_cv_zaptel_vldtmf}" = "yes"; then
	PBX_ZAPTEL_VLDTMF=1
   fi
   AC_MSG_CHECKING(for ZT_TCOP_ALLOCATE in zaptel/zaptel.h)
   saved_cppflags="${CPPFLAGS}"
   if test "x${ZAPTEL_DIR}" != "x"; then
      CPPFLAGS="${CPPFLAGS} -I${ZAPTEL_DIR}/include"
   fi
   AC_COMPILE_IFELSE(
	[
	AC_LANG_PROGRAM(
	[#include <zaptel/zaptel.h>],
	[int foo = ZT_TCOP_ALLOCATE;])
	],
	[	AC_MSG_RESULT(yes) 
		ac_cv_zaptel_transcode="yes" 
	],
	[	AC_MSG_RESULT(no) 
		ac_cv_zaptel_transcode="no" 
	]
	)
   CPPFLAGS="${saved_cppflags}"
   if test "${ac_cv_zaptel_transcode}" = "yes"; then
	PBX_ZAPTEL_TRANSCODE=1
   fi
fi
AC_SUBST(PBX_ZAPTEL_VLDTMF)
AC_SUBST(PBX_ZAPTEL_TRANSCODE)

EDITLINE_LIB=""
if test "x$TERMCAP_LIB" != "x" ; then
  EDITLINE_LIB="$TERMCAP_LIB"
elif test "x$TINFO_LIB" != "x" ; then
  EDITLINE_LIB="$TINFO_LIB"
elif test "x$CURSES_LIB" != "x" ; then
  EDITLINE_LIB="$CURSES_LIB"
elif test "x$NCURSES_LIB" != "x" ; then
  EDITLINE_LIB="$NCURSES_LIB"
else
  AC_MSG_ERROR(*** termcap support not found)
fi
AC_SUBST(EDITLINE_LIB)

AC_CHECK_HEADER([h323.h], [PBX_H323=1], [PBX_H323=0])
AC_SUBST(PBX_H323)

AC_CHECK_HEADER([linux/compiler.h],
                [AC_DEFINE_UNQUOTED([HAVE_LINUX_COMPILER_H], 1, [Define to 1 if your system has linux/compiler.h.])])

AC_CHECK_HEADER([linux/ixjuser.h], [PBX_IXJUSER=1], [PBX_IXJUSER=0], [
				   #include <linux/version.h>
				   #ifdef HAVE_LINUX_COMPILER_H
				   #include <linux/compiler.h>
				   #endif
				   ])
AC_SUBST(PBX_IXJUSER)

PBX_GTK=0
AC_CHECK_TOOL(GTKCONFIG, gtk-config, No)
if test ! "x${GTKCONFIG}" = xNo; then
   GTK_INCLUDE=$(${GTKCONFIG} --cflags gthread)
   GTK_LIB=$(${GTKCONFIG} --libs gthread)
   PBX_GTK=1
   AC_DEFINE([HAVE_GTK], 1, [Define if your system has the GTK libraries.])
fi
AC_SUBST(PBX_GTK)
AC_SUBST(GTK_INCLUDE)
AC_SUBST(GTK_LIB)

PBX_GTK2=0
AC_CHECK_TOOL(PKGCONFIG, pkg-config, No)
if test ! "x${PKGCONFIG}" = xNo; then
   GTK2_INCLUDE=$(${PKGCONFIG} gtk+-2.0 --cflags 2>/dev/null)
   GTK2_LIB=$(${PKGCONFIG} gtk+-2.0 --libs)
   PBX_GTK2=1
   AC_DEFINE([HAVE_GTK2], 1, [Define if your system has the GTK2 libraries.])
fi
AC_SUBST(PBX_GTK2)
AC_SUBST(GTK2_INCLUDE)
AC_SUBST(GTK2_LIB)

if test "${USE_CURL}" != "no"; then
   AC_PATH_TOOL([CURL_CONFIG], [curl-config], No)
   if test ! x"${CURL_CONFIG}" = xNo; then
   # check for version
      if test $(printf "%d" 0x$(${CURL_CONFIG} --vernum)) -ge $(printf "%d" 0x070907); then
         CURL_INCLUDE=$(${CURL_CONFIG} --cflags)
         CURL_LIB=$(${CURL_CONFIG} --libs)

         AC_MSG_CHECKING(for curl_version() in curl/curl.h)
         saved_cppflags="${CPPFLAGS}"
         CPPFLAGS="${CPPFLAGS} ${CURL_INCLUDE}"
         AC_COMPILE_IFELSE(
            [AC_LANG_PROGRAM(
                [#include <curl/curl.h>],
                    [curl_version();])
            ],[
                AC_MSG_RESULT(yes)
                ac_cv_curl_h="yes"
            ],[
                AC_MSG_RESULT(no)
                ac_cv_curl_h="no"
            ]
	 )
	 CPPFLAGS="${saved_cppflags}"
         if test "${ac_cv_curl_h}" = "yes"; then
             PBX_CURL=1
             AC_DEFINE([HAVE_CURL], 1, [Define if your system has the curl libraries.])
         fi
      fi
   fi
fi

AC_CONFIG_FILES([build_tools/menuselect-deps makeopts channels/h323/Makefile])
AC_OUTPUT

if test "x${silent}" != "xyes" ; then
echo
echo "               .\$\$\$\$\$\$\$\$\$\$\$\$\$\$\$=..      "
echo "            .\$7\$7..          .7\$\$7:.    "
echo "          .\$\$:.                 ,\$7.7   "
echo "        .\$7.     7\$\$\$\$           .\$\$77  "
echo "     ..\$\$.       \$\$\$\$\$            .\$\$\$7 "
echo "    ..7\$   .?.   \$\$\$\$\$   .?.       7\$\$\$."
echo "   \$.\$.   .\$\$\$7. \$\$\$\$7 .7\$\$\$.      .\$\$\$."
echo " .777.   .\$\$\$\$\$\$77\$\$\$77\$\$\$\$\$7.      \$\$\$,"
echo " \$\$\$~      .7\$\$\$\$\$\$\$\$\$\$\$\$\$7.       .\$\$\$."
echo ".\$\$7          .7\$\$\$\$\$\$\$7:          ?\$\$\$."
echo "\$\$\$          ?7\$\$\$\$\$\$\$\$\$\$I        .\$\$\$7 "
echo "\$\$\$       .7\$\$\$\$\$\$\$\$\$\$\$\$\$\$\$\$      :\$\$\$. "
echo "\$\$\$       \$\$\$\$\$\$7\$\$\$\$\$\$\$\$\$\$\$\$    .\$\$\$.  "
echo "\$\$\$        \$\$\$   7\$\$\$7  .\$\$\$    .\$\$\$.   "
echo "\$\$\$\$             \$\$\$\$7         .\$\$\$.    "
echo "7\$\$\$7            7\$\$\$\$        7\$\$\$      "
echo " \$\$\$\$\$                        \$\$\$       "
echo "  \$\$\$\$7.                       \$\$  (TM)     "
echo "   \$\$\$\$\$\$\$.           .7\$\$\$\$\$\$  \$\$      "
echo "     \$\$\$\$\$\$\$\$\$\$\$\$7\$\$\$\$\$\$\$\$\$.\$\$\$\$\$\$      "
echo "       \$\$\$\$\$\$\$\$\$\$\$\$\$\$\$\$.                "
echo
fi

AC_MSG_NOTICE(Package configured for: )
AC_MSG_NOTICE( OS type  : $host_os)
AC_MSG_NOTICE( Host CPU : $host_cpu)
if test "${cross_compiling}" = "yes"; then
   AC_MSG_NOTICE( Cross Compilation = YES)
fi
