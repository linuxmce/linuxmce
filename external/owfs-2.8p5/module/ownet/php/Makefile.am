## $Id: Makefile.am,v 1.6 2007/05/21 15:58:53 d1mag Exp $

EXTRA_DIST = ownet.php examples/bcadd.php examples/ownet_example.php.in examples/ownet_example.php

install-data-local:
	@INSTALL@ -d ${DESTDIR}${prefix}/bin/
	@INSTALL@ ownet.php ${DESTDIR}${prefix}/bin/
#	@INSTALL@ examples/bcadd.php ${DESTDIR}${prefix}/bin/

clean-generic:

	@RM@ -f *~ .*~
