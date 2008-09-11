all: gettext-input sqlcvsweb.po lang-de

gettext-input:
	find . -name "*.php" > gettext.input
	find . -name "*.inc" >> gettext.input

sqlcvsweb.po:
	xgettext -f gettext.input -C -o sqlcvsweb.po --keyword=_

lang-de:
	msgfmt sqlcvsweb-de.po --output-file=locale/de_DE/LC_MESSAGES/sqlcvsweb.mo
