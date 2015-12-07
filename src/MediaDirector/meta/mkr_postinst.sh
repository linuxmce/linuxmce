#!/bin/sh

. /usr/pluto/bin/Config_Ops.sh

[[ -z "$AutostartMedia" ]] && ConfSet "AutostartMedia" "1"
[[ -z "$AVWizardDone" ]] && ConfSet "AVWizardDone" "0"
[[ -z "AVWizardOverride ]] && ConfSet "AVWizardOverride" "1"

exit 0
