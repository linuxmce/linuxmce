#!/bin/sh

## 
## Setup Script for the Phoenix Solo USB microphone.
##
## TODO: Soundcard detection needs to be refined so that
## It doesn't just assume it's the second sound device
## in the system.
##

## AudioSettings parameter isn't used at all. This is a completely
## separate device from the main sound in an MD.

amixer -c 1 sset "PCM",0 50 >/dev/null 
amixer -c 1 sset "Speaker",0 44 >/dev/null
amixer -c 1 sset "Mic",0 50 >/dev/null

