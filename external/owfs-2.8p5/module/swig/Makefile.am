# $Id: Makefile.am,v 1.15 2009/07/01 01:26:27 alfille Exp $

EXTRA_DIST = ow.i

if ENABLE_OWPERL
if ENABLE_PERL
  SWIG_SUBDIRPERL = perl5
endif
endif

if ENABLE_OWPYTHON
if ENABLE_PYTHON
  SWIG_SUBDIRPYTHON = python
endif
endif

if ENABLE_OWPHP
if ENABLE_PHP
  SWIG_SUBDIRPHP = php
endif
endif

SUBDIRS = $(SWIG_SUBDIRPERL) $(SWIG_SUBDIRPYTHON) $(SWIG_SUBDIRPHP)

clean-generic:

	@RM@ -f *~ .*~
