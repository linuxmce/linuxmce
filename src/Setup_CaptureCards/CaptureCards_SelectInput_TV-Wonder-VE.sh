#!/bin/bash

DevEntry="$1" #/dev/videoX
Input="$2"

# hardcoded input values as they are in the database
# also, positions in the Inputs array

Input_Antenna=0
Input_Composite=1



# Inputs=("" Tuner,Composite)
Inputs=("" 0 1)

v4l2-ctl --device="$DevEntry" --set-input="${Inputs[$Input]}"
