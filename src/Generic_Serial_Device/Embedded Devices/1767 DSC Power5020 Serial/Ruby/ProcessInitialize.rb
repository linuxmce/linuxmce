#ProcessInitialize  2006-11-08 12:08  Power5020

#create log file
#$logFile = File.new("/var/log/pluto/DSC_Power.log", "w")
$logFile = File.new("/var/log/pluto/DSCPower5020.log", "w")
#global variables
$waitTime = 400      #wait time in comunication
$bFlush = true        #log imediatly every event,comand

#panel settings
$MaxSensor=64
$MaxPartition=8
$MinLineLength=7
$MaxLineLength=30
$bDescArm=false
$bTimeStamp=false
$bTimeBroadcast=false

$bPartDetect=true    #?

#buffer for reading from serial
$line=String.new
$lineIndex=0

#sensor and partition state
$sensorStatus=Hash.new
$sensorType=Hash.new
$partStatus=Hash.new
#partition mapping
$partMapping=Hash.new
#description of partition and zone
$partStr=String.new
$zoneStr=String.new

#state of panel  0-start,1-init,2-ready to receive command,3 analize to receivce 
$panelState=0
$bInit=false
#command buffer and 
$cmdBuffer=Array.new
$bCmdSuccess=true
$timeNo=0
$lastCmdTime=Time.now()

#$myIo = MyIO.new

$cmdCode = { 
"001"    =>    "Status report",
"010"    =>    "Set date time",
"020"    =>    "Command output control",
"03"      =>   "Arm partition",
"040"    =>   "Disarm partition",
"055"    =>   "Set time stamp",
"056"    =>   "Time brodcast",
"057"    =>   "Temperature brodcast",
"060"    =>   "Simulate alarm",
"200"    =>   "Code send",
"500"    =>    "Comand acknolegde",
"501"    =>    "Command error",
"502"    =>    "System error",
"550"    =>    "Receive time/date",
"609"    =>    "Zone open",
"610"    =>    "Zone restore",
"650"    =>    "Partition ready",
"651"    =>    "Partition not ready",
"652"    =>    "Partition armeded" ,
"654"    =>    "Partition in alarm", 
"655"    =>    "Partition disarmed", 
"656"    =>    "Exit delay",
"657"    =>    "Entry delay",
"840"    =>    "Trouble status"
}

$errCode = {
"000"     =>  "No error",
"001"     =>  "RS-232 Receive  buffer over a command is received while another is still processing",
"002"     =>  "RS-232 Receive  buffer overflow",
"003"     =>  "Keybus transmit buffer over",
"010"     =>  "Keybus transmit buffer overflow",
"011"     =>  "Keybus transmit",
"012"     =>  "Keybus transmit",
"013"    =>   "Keybus transmit",
"014"    =>   "Keybus",
"015"    =>   "Keybus",
"016"    =>   "Keybus",
"017"    =>   "Keybus",
"020"   =>    "API Command syntax error",
"021"   =>    "API Command partition error",
"022"   =>    "API Command not suported",
"023"   =>    "API System not armed" ,
"024"   =>    "API System not ready to arm",
"025"   =>    "API Command invalid length",
"026"   =>    "API User code not required",
"027"   =>    "API Invalid characters in command"
}

#if device has childs already initialise
$partStatus[1]=true
if (device_.childdevices_ == nil) or (device_.childdevices_.empty?) then
	$bInit = false
else 
	$bInit = true 
end

log( "Starting DSC Power5020\n" )
logState( true )

$DEVICEDATA_Zones_CONST = 135
if (device_.devdata_ != nil) then
	if	( device_.devdata_.has_key?($DEVICEDATA_Zones_CONST) ) and
		( device_.devdata_[$DEVICEDATA_Zones_CONST] != nil) and
		( !device_.devdata_[$DEVICEDATA_Zones_CONST].empty? ) then
		$logFile.print "Mapping:" + device_.devdata_[$DEVICEDATA_Zones_CONST] + "\n"
		mapp( device_.devdata_[$DEVICEDATA_Zones_CONST] )
	else
		$logFile.print "Mapping with empty string" + "\n"
		mapp("")
	end
else
	$logFile.print "Run without devicedata " + "\n"
	mapp( "" )
end
#mapp("")

log( "\n\n" )

