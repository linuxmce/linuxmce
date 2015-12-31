#31-May-06 14:13 init Denon AVC
$logFile = File.new("/var/log/pluto/DenonAVC.log", "w")
$SoundLevel = 0

#buffer for reading from serial
$line=String.new
$lineIndex=0

$waitTime = 3000      #wait time in comunication

$eventCode = { 
"MS"    =>    "Mode surround",
"PS"    =>    "Parameter settings",
"TF"    =>    "Tone frequency",
"TP"    =>     "Tuner present",
"TM"    =>     "Tuner band",
"PW"    =>     "Power",
"MV"    =>     "Master volume",
"CV"    =>     "Chanel volume",
"MU"    =>     "Mute",
"SI"    =>    "Select Input"
}

$inputSources = {
"VDP"              =>        176,
"VCR-3"           =>        284,
"VCR-2"           =>        283,
"VCR-1"           =>        282,
"V.AUX"           =>        285,
"TV"                =>       161,
"TUNER"           =>       166,
"PHONO"           =>       163,
"MD/TAPE2"      =>       174,
"DVD"              =>       165,
"DBS/SAT"        =>       281,
"CD"                =>       162,
"CDR/TAPE1"     =>       420
}
