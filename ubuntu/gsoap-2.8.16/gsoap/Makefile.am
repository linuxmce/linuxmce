## This is the input file for automake, which will generate Makefile.in ##
## not a GNU package. You can remove this line, if
## you have all needed files, that a GNU package needs
AUTOMAKE_OPTIONS = foreign 1.4

SUBDIRS = . src wsdl @SAMPLE_DIRS@
CLEANFILES = soapcpp2 wsdl2h stdsoap2_cpp.cpp stdsoap2_ck.c stdsoap2_ck_cpp.cpp stdsoap2_ssl.c stdsoap2_ssl_cpp.cpp dom_cpp.cpp

##	*~ *.req.xml *.res.xml *.nsmap *.wsdl *.xsd *Proxy.h \
##	*.o soapH.h soapStub.h soapC.cpp soapC.c soapClient.cpp soapClient.c soapServer.cpp soapServer.c

AM_CXXFLAGS = $(SOAPCPP2_DEBUG) -D$(platform)
AM_CFLAGS = $(SOAPCPP2_DEBUG) -D$(platform)

# Install all soapcpp2 and wsdl2h files into ${prefix}/share/gsoap
nobase_pkgdata_DATA = $(srcdir)/import/* $(srcdir)/plugin/* $(srcdir)/WS/* $(srcdir)/custom/*

## we cannot build stdsoap2.o from 2 different sources (stdsoap2.cpp and stdsoap2.c), so we need an intermediate target:
stdsoap2_ck.c: stdsoap2.cpp
	$(LN_S) -f $(srcdir)/stdsoap2.cpp stdsoap2_ck.c

stdsoap2_cpp.cpp: stdsoap2.cpp
	$(LN_S) -f $(srcdir)/stdsoap2.cpp stdsoap2_cpp.cpp

stdsoap2_ck_cpp.cpp: stdsoap2.cpp
	$(LN_S) -f $(srcdir)/stdsoap2.cpp stdsoap2_ck_cpp.cpp

stdsoap2_ssl.c: stdsoap2.cpp
	$(LN_S) -f $(srcdir)/stdsoap2.cpp stdsoap2_ssl.c

stdsoap2_ssl_cpp.cpp: stdsoap2.cpp
	$(LN_S) -f $(srcdir)/stdsoap2.cpp stdsoap2_ssl_cpp.cpp
## do the same fo dom.cpp since we want to link it into the cpp libs and dom.c into the c libs
dom_cpp.cpp: dom.cpp
	$(LN_S) -f $(srcdir)/dom.cpp dom_cpp.cpp

lib_LTLIBRARIES = libgsoap.la libgsoap++.la libgsoapck.la libgsoapck++.la libgsoapssl.la libgsoapssl++.la

SOVERSION = 0

libgsoap_la_SOURCES = stdsoap2.c dom.c
libgsoap_la_CFLAGS = $(SOAPCPP2_DEBUG) $(SOAPCPP2_NONAMESPACES) $(SOAPCPP2_IPV6) -D$(platform)
libgsoap_la_LDFLAGS = -version-info $(SOVERSION)
libgsoap___la_SOURCES = stdsoap2_cpp.cpp dom_cpp.cpp
libgsoap___la_CXXFLAGS = $(SOAPCPP2_DEBUG) $(SOAPCPP2_NONAMESPACES) $(SOAPCPP2_IPV6) -D$(platform)
libgsoap___la_LDFLAGS = -version-info $(SOVERSION)
libgsoapck_la_SOURCES = stdsoap2_ck.c dom.c
libgsoapck_la_CFLAGS = $(SOAPCPP2_DEBUG) $(SOAPCPP2_NONAMESPACES) $(SOAPCPP2_IPV6) -D$(platform) -DWITH_COOKIES
libgsoapck_la_LDFLAGS = -version-info $(SOVERSION)
libgsoapck___la_SOURCES = stdsoap2_ck_cpp.cpp dom_cpp.cpp
libgsoapck___la_CXXFLAGS = $(SOAPCPP2_DEBUG) $(SOAPCPP2_NONAMESPACES) $(SOAPCPP2_IPV6) -D$(platform) -DWITH_COOKIES
libgsoapck___la_LDFLAGS = -version-info $(SOVERSION)
libgsoapssl_la_SOURCES = stdsoap2_ssl.c dom.c
libgsoapssl_la_CFLAGS = $(SOAPCPP2_DEBUG) $(SOAPCPP2_NONAMESPACES) $(SOAPCPP2_IPV6) -D$(platform) $(WSDL2H_EXTRA_FLAGS) -DWITH_DOM
libgsoapssl_la_LDFLAGS = -version-info $(SOVERSION)
libgsoapssl_la_LIBADD = -lssl -lcrypto -lz
libgsoapssl___la_SOURCES = stdsoap2_ssl_cpp.cpp dom_cpp.cpp
libgsoapssl___la_CXXFLAGS = $(SOAPCPP2_DEBUG) $(SOAPCPP2_NONAMESPACES) $(SOAPCPP2_IPV6) -D$(platform) $(WSDL2H_EXTRA_FLAGS) -DWITH_DOM
libgsoapssl___la_LDFLAGS = -version-info $(SOVERSION)
libgsoapssl___la_LIBADD = -lssl -lcrypto -lz

BUILT_SOURCES = stdsoap2_cpp.cpp dom_cpp.cpp $(lib_LIBRARIES)

include_HEADERS = stdsoap2.h

install-data-hook:
	$(LN_S) -f $(top_builddir)/gsoap/src/soapcpp2 $(top_builddir)/gsoap/soapcpp2 || echo "ok, link already exists".
	$(LN_S) -f $(top_builddir)/gsoap/wsdl/wsdl2h $(top_builddir)/gsoap/wsdl2h || echo "ok, link already exists".

