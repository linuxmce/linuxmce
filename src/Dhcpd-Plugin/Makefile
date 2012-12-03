#<-dceag-mh-b->
#<-dceag-mh-e->

sources += \
configure_aastra.pl \
configure_cisco7910.pl \
configure_cisco7921.pl \
configure_cisco7941.pl \
configure_cisco7960.pl \
configure_cisco7970.pl \
configure_cisco_phone.pl \
configure_grandstream.pl \
configure_gs_de380_ip_r.pl \
configure_iaxy.pl \
configure_snom3xx.pl \
configure_snom.pl \
configure_squeezebox.pl \
Dhcpd-Plugin.pl \
sync_pluto2amp.pl \
Dhcpd-Plugin-Parser.sh \
Dhcpd-Plugin-Queue.sh \
Dhcpd-Plugin.sh


#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

all:
	/bin/true 

distclean: clean

clean: clean-recursive
	/bin/true

package:
	/bin/true

#<-dceag-mf-e->


install: 
	mkdir -p $(DESTDIR)/usr/pluto/bin
	cp $(sources) $(DESTDIR)/usr/pluto/bin

