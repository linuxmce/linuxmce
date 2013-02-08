########################################################################
# Description : Pluto parallel startup using makefiles
# Author      : Razvan Gavril <razvan.g@plutohome.com>
# License     : GPL
#
#    make [-n] -i -j -f main.mk RUNLEVEL={0|1|2|3|4|5|6} JOB={start|stop|restart|status}
#
#    PS: $(JOB) is not validated - that is left to the service program.
#
########################################################################

# passed as a parameter (0, 1, 2, 3, 4, 5, 6)
RUNLEVEL = 

# passed as a parameter (start, stop, status, etc)
JOB =

########################################################################
# START CONFIGURATION

# system commands used by this facility
CAT               = /bin/cat
RM                = /bin/rm
ECHO              = /bin/echo
DATE              = /bin/date
TPUT	          = /usr/bin/tput
# Directory containing scripts/programs to run.
INITD_DIR        := /etc/init.d

# Directory into which a lock file is created when a service starts.
# (Note that the lock file is created by the service).
SUBSYS_FILE_DIR  := /var/lock/subsys

# Used to create temporary files, before collating them all into 
TMP_DIR          := /var/run/plutoboot

TMPFILE_PREFIX   := .runlevel
TMP_FILE          = $(TMP_DIR)/$(TMPFILE_PREFIX).$(JOB).$@


# List of *all* services.
# (Important Note: if you don't include a service in this list,
# it won't get run!)
include /etc/init.d/fastboot/services.mk

# END CONFIGURATION
########################################################################

ifndef RUNLEVEL
$(error must specity a RUNLEVEL, so I know what to run)
endif

ifndef JOB
$(error must specify JOB, so I know what to do)
endif

default: $(ALL) 

##############################################################
# Generic rule to control a service.
#
# Note that we capture all output to a file.

$(ALL)	: $(SUBSYS_FILE_DIR)/$@
#	@$(ECHO) -e "\033[1m# Begin $(JOB) $@ ..."
#	@$(TPUT) sgr0
	@$(ECHO) "Begin \"$(JOB) $@\" at `$(DATE)`"  > $(TMP_FILE)
	@$(INITD_DIR)/$@ $(JOB)                      >> $(TMP_FILE) 2>&1
	@$(ECHO) "End \"$(JOB) $@\" at `$(DATE)`"    >> $(TMP_FILE)
#	@$(ECHO) -e "\033[1m# Finished $(JOB) $@ ..."
#	@$(TPUT) sgr0

##############################################################
# List of services that have dependencies.
# (Note: It is not necessary to list services that have no
# dependencies).
include /etc/init.d/fastboot/depend/$(RUNLEVEL)-$(JOB).mk

