
# $Id: Makefile 1153 2006-05-17 04:39:09Z isely $

RELPROG=../utils/release_snapshot.pl

all: history.html

FORCE:

history.html: change_history.txt
	$(RELPROG) --genHistoryFile --historySourceFile=$< --historyHTMLFile=$@

# NOTE: This works by directory querying the SVN repository for tags
# matching a pattern.  Thus it will only work if you have access to
# that SVN repository.  That is why this target is not built by
# default.  (But really, running this one only makes sense for the
# isely.net site anyway.)
download.html: FORCE
	$(RELPROG) --genDownloadFile --downloadHTMLFile=$@
