#!/bin/bash

#<-mkr_B_via_b->
#update-rc.d -f check_avwizard remove >/dev/null
#update-rc.d -f a0start_avwizard remove >/dev/null
#update-rc.d -f check_avwizard start 98 2 3 4 5 . >/dev/null
#update-rc.d -f a0start_avwizard start 99 2 3 4 5 . >/dev/null

#if [[ -e /etc/init.d/0start_avwizard ]]; then
#	update-rc.d -f 0start_avwizard remove >/dev/null
#	rm -f /etc/init.d/0start_avwizard
#fi
#if [[ -e /etc/init.d/a0start_avwizard ]]; then
#	update-rc.d -f a0start_avwizard remove >/dev/null
#	rm -f /etc/init.d/a0start_avwizard
#fi
#if [[ -e /etc/init.d/check_avwizard ]]; then
#	update-rc.d -f check_avwizard remove >/dev/null
#	rm -f /etc/init.d/check_avwizard
#fi
#<-mkr_B_via_e->
exit 0
