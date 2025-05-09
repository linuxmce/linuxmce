# FILE: AutoMake Makefile for chan-sccp-b
# COPYRIGHT: chan-sccp-b.sourceforge.net group 2009
# CREATED BY: Diederik de Groot <ddegroot@sourceforge.net>
# LICENSE: This program is free software and may be modified and distributed under the terms of the GNU Public License.
#          See the LICENSE file at the top of the source tree.
# DATE:     $Date: 2011-10-11 17:30:35 +0000 (Tue, 11 Oct 2011) $
# REVISION: $Revision: 2997 $
# NOTE: Process this file with automake to produce a makefile.in script.

include $(top_srcdir)/amdoxygen.am
AUTOMAKE_OPTS			= gnu silent-rules
EXTRA_DIST 			= .version tools conf
#SUBDIRS 			= src doc contrib/gen_sccpconf
SUBDIRS 			= src doc
ACLOCAL_AMFLAGS			= -I autoconf
AM_CPPFLAGS			= $(LTDLINCL)
distdir = $(PACKAGE)-$(SCCP_VERSION)_$(SCCP_BRANCH)
	
# Update libtool if necessary	
libtool: @LIBTOOL_DEPS@
	cd $(srcdir) && \
	$(SHELL) ./config.status --recheck
	
FORCE:

# Update ChangeLog file from SVN Log
ChangeLog:	FORCE
if ENABLE_SVN2CL
	@if svnrev=`svn info |grep Revision`; then \
	  if [ -z "`cat ChangeLog 2>/dev/null| grep "\[r$${svnrev:10}\]"`" ];then \
            echo "Generating ChangeLog from SVN $(CHANGELOG_OLDEST)->HEAD"; \
            $(SVN2CL) --include-actions -i -a --group-by-day --separate-daylogs \
            	--include-rev \
             	--revision-link='http://chan-sccp-b.svn.sourceforge.net/viewvc/chan-sccp-b/trunk/?pathrev=' \
              	--ticket-link='http://sourceforge.net/tracker/?func=detail&group_id=186378&atid=917045&aid=' \
              	--ignore-message-starting='#' \
              	--revision HEAD:$(CHANGELOG_OLDEST); \
	  fi; \
	fi
else
	svn log > ChangeLog
endif

# Update doc/html/ChangeLog.html from SVN Log
ChangeLog-HTML:	FORCE
if ENABLE_SVN2CL
	@if svnrev=`svn info |grep Revision`; then \
	  if [ -z "`cat doc/html/ChangeLog_SVN2CL.html 2>/dev/null| grep "\[r$${svnrev:10}\]"`" ];then \
            echo "Generating doc/html/ChangeLog_SVN2CL.html from SVN $(CHANGELOG_OLDEST)->HEAD"; \
            if [ ! -d doc/html ]; then mkdir doc/html;fi; \
            $(SVN2CL) --include-actions -i -a --group-by-day --title "Chan_SCCP-b ChangeLog" --separate-daylogs --include-rev --ignore-message-starting='#' --revision HEAD:$(CHANGELOG_OLDEST) --html -o doc/html/ChangeLog_SVN2CL.html \
            --revision-link 'http://chan-sccp-b.svn.sourceforge.net/viewvc/chan-sccp-b?view=rev&revision=' \
            --ticket-link='http://sourceforge.net/tracker/?func=detail&group_id=186378&atid=917045&aid='; \
            cp autoconf/svn2html.css doc/html/; \
	  fi; \
	fi
endif

# Update ChangeLog Files and Generate Doxygen Documentation
doxygen: ChangeLog ChangeLog-HTML doxygen-doc

deb: 
	dpkg-buildpackage -rfakeroot -us -uc

rpm:    release
	@if test ! -z "$(RPMBUILD)" ; then \
	  cp $(distdir).tar.gz /usr/src/packages/SOURCES/; \
	  cp tools/chan-sccp-b.spec /usr/src/packages/SOURCES/; \
	  export DISTDIR=${distdir}; \
	  if [ -f /etc/SuSE-release ]; then \
                  if [ "$(ASTERISK_VER)" == "1.4" ]; then \
                          export ASTERISKVER=""; \
                  else \
                          export ASTERISKVER="`echo $(ASTERISK_VER)|sed 's/\.//g'`"; \
                  fi; \
	  fi; \
          export RPMVERSION="$(SCCP_VERSION)"; \
          export RPMBRANCH="$(SCCP_BRANCH)"; \
          export RPMREVISION="$(SCCP_REVISION"; \
          cd /usr/src/packages/SOURCES/ && rpmbuild -bb chan-sccp-b.spec; \
        else \
	  echo ""; \
	  echo "RPM Build on your platform not (yet) supported !"; \
	  echo "If you like you could send us the required package build file for your platform"; \
	  echo ""; \
	fi

release:
	@rm -f ChangeLog
	@$(MAKE) ChangeLog
	@if [ "`grep -i $(SCCP_VERSION)_$(SCCP_BRANCH) NEWS`" == "" ]; then \
	  echo >>NEWS;\
	  echo "Version: $(SCCP_VERSION)_$(SCCP_BRANCH)" >> NEWS; \
	  echo "Please edit the NEWS file now..."; \
	  /usr/bin/joe NEWS; \
	  /usr/bin/debchange; \
	fi 
	@$(MAKE) dist-gzip

gen_sccpconf:
	cd contrib/gen_sccpconf && $(MAKE) $(AM_MAKEFLAGS) all
