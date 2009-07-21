#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh
. /usr/pluto/bin/Config_Ops.sh 2>/dev/null || exit 1  ## So the orbiter can get the environment variable for the mysql server

Done=0
NextStep="Welcome"
export DISPLAY=:$(( $Display  + 1 ))
while [[ "$Done" -eq 0 ]]; do
	echo "Running Wizard from step '$NextStep'"
	StartX -step "$NextStep"
	Ret=$(WizGet ExitCode)
	echo "Wizard exited with code: $Ret"
	case "$Ret" in
		2) : ;; # programmed exit
		*) exit $Ret ;; # 0 = I agree; 1 = I do not agree; 3 = Skip Wizard; * = other
	esac
	WizStep=$(WizGet InterruptedStep)

	if [[ -f /tmp/avwizard-resolution.txt ]]; then
		WizResolution=$(< /tmp/avwizard-resolution.txt)
		VideoResolution_Name=$(SpcField 1 "$WizResolution")
		VideoRefresh=$(SpcField 2 "$WizResolution")
		WindowWidth=$(SpcField 3 "$WizResolution")
		WindowHeight=$(SpcField 4 "$WizResolution")
		Connector=$(SpcField 5 "$WizResolution")
		TVStandard=$(SpcField 6 "$WizResolution")

		set -x
		WizSet VideoResolution "$VideoResolution_Name"
		WizSet VideoRefresh "$VideoRefresh"
		WizSet WindowWidth "$WindowWidth"
		WizSet WindowHeight "$WindowHeight"
		WizSet VideoOutput "$Connector"
		WizSet TVStandard "$TVStandard"
		set +x
	fi

	echo "Interrupted in step '$WizStep'"

	case "$WizStep" in
		"Welcome/USR1")
			NextStep="Welcome"
			if [[ ! -f /tmp/AVWizard_Started ]]; then
				rm -f /tmp/avwizard-resolution-defaults.txt
				/usr/pluto/bin/AVWizard_UpdateResolution.sh reset
				beep -f 500 -l 400 -r 5
			fi
			rm -f /tmp/AVWizard_Started
		;;
		"VideoResolution/USR1") NextStep="VideoResolutionConfirm" ;;
		"VideoResolution/USR2") NextStep="VideoResolution" ;;
		"VideoResolutionConfirm"*)
			NextStep="VideoResolution"
			/usr/pluto/bin/AVWizard_UpdateResolution.sh reset
		;;
		*)
			echo "Interrupted in step '$WizStep', but shouldn't be"
			NextStep="Welcome" # TODO: reimplement "go to step-1"
			#if [[ "$WizStep" -lt 0 ]]; then
			#	((WizStep=-WizStep))
			#fi
			#((NextStep=WizStep-1))
			#if [[ "$NextStep" -lt "$STEP_Welcome" ]]; then
			#	NextStep=$STEP_Welcome
			#fi
		;;
	esac

	# if AVWizard fails to start at a certain step, like the video confirmation step,
	# ensure that it is treated as if it was interrupted, not the previous step
	WizSet InterruptedStep "$NextStep"
	
	sleep 2 # things seem to mess up at random when X is stopped and started too fast
done
