#!/bin/bash

#<-mkr_B_via_b->
update-rc.d check_avwizard start 98 S . >/dev/null
update-rc.d a0start_avwizard start 99 2 . >/dev/null 
	if [[ -e /etc/init.d/0start_avwizard ]]; then 
		update-rc.d -f 0start_avwizard remove 
		rm /etc/init.d/0start_avwizard 
	fi 
#<-mkr_B_via_e->
