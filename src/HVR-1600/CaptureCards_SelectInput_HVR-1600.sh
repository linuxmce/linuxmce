#!/bin/bash

DevEntry="$1" #/dev/videoX
Input="$2"

# hardcoded input values as they are in the database
# also, positions in the Inputs array

Input_Antenna=1
Input_SVideo=2
Input_Composite=3
Input_SVideo=4
Input_Composite=5



# Inputs=("" Tuner 1, S-Video 1, Composite 1, S-video 2, Composite 2)
Inputs=("" 0 1 2 3 4)

v4l2-ctl --device="$DevEntry" --set-input="${Inputs[$Input]}"
