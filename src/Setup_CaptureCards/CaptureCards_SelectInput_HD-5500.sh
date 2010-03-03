#!/bin/bash

DevEntry="$1" #/dev/videoX
Input="$2"

# hardcoded input values as they are in the database
# also, positions in the Inputs array

Input_Component=1
Input_SVideo=2
Input_Composite=3



# Inputs=("" Component, S-video, Composite)
Inputs=("" 0 1 2)

v4l2-ctl --device="$DevEntry" --set-input="${Inputs[$Input]}"
