####### This is the input file for automake, which will generate Makefile.in ##########


## not a GNU package. You can remove this line, if
## you have all needed files, that a GNU package needs
AUTOMAKE_OPTIONS = foreign 1.4

INCLUDES=-I$(top_srcdir)/gsoap -I$(top_srcdir)/gsoap/plugin
AM_LDFLAGS=$(INCLUDES) -I$(srcdir) -L$(srcdir)
SOAP=$(top_srcdir)/gsoap/src/soapcpp2
WSDL=$(top_srcdir)/gsoap/wsdl/wsdl2h
WSDL_FLAGS=-g -t $(srcdir)/addresstypemap.dat
SOAP_FLAGS=-0 -CS -p address -I$(top_srcdir)/gsoap/import
WSDLINPUT=$(srcdir)/address.xsd
SOAPHEADER=$(srcdir)/address.h
SOAP_CPP_SRC=addressC.cpp
SOAP_CPP_LIB=$(top_builddir)/gsoap/libgsoap++.la

$(SOAP_CPP_SRC) : $(WSDLINPUT)
	$(WSDL) $(WSDL_FLAGS) $(WSDLINPUT)
	$(SOAP) $(SOAP_FLAGS) $(SOAPHEADER)

#LIBS=

noinst_PROGRAMS=address

address_CFLAGS=$(C_DEBUG_FLAGS) -D$(platform)
address_CPPFLAGS=$(C_DEBUG_FLAGS) -D$(platform)
address_CXXFLAGS=$(WSDL2H_EXTRA_FLAGS) $(WSDL2H_IMPORTPATH)
address_SOURCES=$(SOAP_CPP_SRC) address.cpp
address_LDADD=$(SOAP_CPP_LIB)

CLEANFILES= *~ *C.cpp *H.h *Stub.h *.nsmap
