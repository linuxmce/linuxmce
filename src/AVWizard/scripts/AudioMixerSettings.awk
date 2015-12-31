#!/usr/bin/awk

/Limits:/ { printf "AudioVolumeMin=%d; AudioVolumeMax=%d; ", $3, $5 }
/Front Left:/ { printf "AudioVolumeCurrent=%d; ", $4 }
