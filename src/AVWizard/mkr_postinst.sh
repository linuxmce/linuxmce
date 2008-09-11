#!/bin/bash

#<-mkr_B_via_b->
update-rc.d check_avwizard start 98 S . >/dev/null
update-rc.d 0start_avwizard start 99 2 . >/dev/null
#<-mkr_B_via_e->
