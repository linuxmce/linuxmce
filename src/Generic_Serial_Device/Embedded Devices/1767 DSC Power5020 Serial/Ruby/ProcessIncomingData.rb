#Event process  20-Jul-06 16:30 Power5020

#read event code from serial
readLine()

if ($line.size>17) and ($line.size<=26) and ($bTimeStamp==false) then       #already set time stamp
	log( "\n\n" + "Already time stamp" + "\n\n" )
	$bTimeStamp=true
end

if ($bTimeStamp == true) then 
	panelTime=readByte(9)
	log( "Security panel time:" + panelTime + "\n" )
end

cmd=readByte(3)
logCmd(cmd)
cmdNo=cmd.to_i

case cmd
when "500"     #Command is ok. Read 3 bytes code of succesfully command
	param=readByte(3)
	log( "Command is ok: " + param + " " )
when "501"     #Command error
	log( "Command error: " )
when "502"     #System error. Read 3 bytes for code error
	param=readByte(3)
	DSCError(param)
when "550"    #Timedata HH::MM MM/DD/YY
	$timeNo += 1
	myData=readByte(10)
	log( "Time:" + myData + " No:" + $timeNo.to_s + "\n" )
	
	if ($bTimeBroadcast == false) then       #already in  time broadcast
		log(  "\n" + "Already in time broadcast" + "\n" )
		$bTimeBroadcast=true
	end

	if ($timeNo < 2)  then    #if send ReportCommand later 
		log( "Call addChildDevice " + "\n" )
		addChildDevices() 
	end
when "560"    #Ring detected
	log( "Ring detected:" )
when "561"    #Indoor temperature 1 byte termostat 3 bytes temperature
	data=readByte(4)
	log( "Indoor temperature: " + data )
when "562"    #Outdoor temperature 1 byte termostat 3 bytes temperature
data=conn.Recv(4)
log( "Outdoor temperature:" )
when "601"    #Zone alarm 1 byte partition 3 bytes zone
partNo=readByte(1)
zoneNo=readByte(3)
log( "Zone alarm: " + partNo + "," + zoneNo )
when "602"    #Zone alarm restauration 1 byte partition,3 bytes zone
partNo=readByte(1)
zoneNo=readByte(3)
log( "Zone alarm restore: " + partNo + "," + zoneNo )
when "603"    #Zone tamper 1 byte partititon 3 bytes zone
partNo=readByte(1)
zoneNo=readByte(3)
log( "Zone tamper: " + partNo + "," + zoneNo )
when "604"    #Zone tamper restore 1 byte partititon 3 bytes zone
partNo=readByte(1)
zoneNo=readByte(3)
log( "Zone alarm:" + partNo + "," + zoneNo )
when "605"    #partition fault. 1 byte partition      
partNo=readByte(1)  
log( "Partition fault:" + partNo )
when "606"    #partition fault restore. 1 byte partition     
partNo=readByte(1)
log( "Partition fault restore:" + partNo )

when "609"    #Zone open.  3 bytes zone 
	zoneNo=readByte(3)
	log( "Zone number: " + zoneNo + "  " )
	addSens(zoneNo,true,54)    #54 motion detector
when "610"    #Zone restore. 3 bytes zone
	zoneNo=readByte(3)
	log( "Zone number:" + zoneNo + "  " )
	addSens(zoneNo,false,54)    #54 motion detector

when "620"    #Duress alarm 4 bytes
data=readByte(4)
log( "Duress alarm: " + data )

when "621"    #Fire alarm 
log( "Fire alarm:" )
when "622"    #Fire alarm restore
log( "Fire alarm restore:" )
when "623"    #Auxiliary alarm
log( "Auxiliary alram:" )
when "624"    #Auxiliary alarm restore
log( "Auxiliary alarm restore:" )
when "625"    #Panic alarm
log( "Panic alarm:" )
when "626"    #Panic alarm restore
log( "Panic alarm restore:" )
when "631"    #Smoke alarm
log( "Smoke alarm:" )
when "632"    #Smoke alarm restore
log( "Smoke alarm restore:" )
when "650"    #Partition ready. 1 byte partition
	partNo=readByte(1)
	log( "Partition no: " + partNo + "  " )
	addPart(partNo,true)
when "651"    #Partition not ready. 1 byte partition
	partNo=readByte(1)
	log( "Partition no:" + " No:" + partNo + "  " )
	addPart(partNo,false)
when "652"    #pPartition armed. 1 byte partition
	partNo=readByte(1)
	mode="-1"
	if ($bPartDec == true) then mode=readByte(1) end
	log( "Partition no:" + partNo + " Mode:" + mode.to_s + "   " )
	addPart(partNo,true)
	
	#change state to arm without error
	changeStateEv= Command.new(device_.devid_, -1001, 1, 2, 67); 
	changeStateEv.params_[30]="1"
	changeStateEv.params_[47]=partNo.to_s
	SendCommand(changeStateEv)
when "654"    #Partition in alarm. 1 byte partition   
	partNo=readByte(1)
	log( "Partition no: " + partNo )
when "655"    #Partition disarm. 1 byte partition
	partNo=readByte(1)
	log( "Partition no: " + partNo )
	
	#change state to disarm without error
	changeStateEv= Command.new(device_.devid_, -1001, 1, 2, 67); 
	changeStateEv.params_[30]="0"
	changeStateEv.params_[47]=partNo.to_s
	SendCommand(changeStateEv)
when "656"    #Exit delay    1 byte partition
	partNo=readByte(1)
	log( "Part no:" + partNo + "  " )
when "657"    #Eentry delay  1 byte partition
	partNo=readByte(1)
	log( "Part no: " + partNo + "  " )
when "658"    #Keypad lock out 1 byte partition
partNo=readByte(1)
log( "Keypad lock out: " + partNo )
when "670"    #Invalid code access 1 byte partition
partNo=readByte(1)
log( "Invalid code: " + partNo )
when "671"    #Function not available 1 byte partition
partNo=readByte(1)
log( "Function not available: " + partNo )
when "700"    #User closing 1 byte partition 4 bytes user
partNo=readByte(1)
userNo=readByte(4)
log( "User closing: " + partNo + "," + userNo )
when "701"    #Special closing 1 byte partition
partNo=readByte(1)
log( "Special closing: " + partNo )
when "702"    #Partial closing 1 byte partition
partNo=readByte(1)
log( "Partial closing: " + partNo )
when "750"    #User opening  1 byte partition 4 bytes user
partNo=readByte(1)
userNo=readByte(4)
log( "User opening: " + partNo + "," +userNo )
when "751"    #Special opening 1 byte partition
partNo=readByte(1)
log( "Special opening: " + partNo )
#miss a part of events
when "816"    #Buffer near full
log( "Buffer near full:" )
when "821"    #Device low batery  3 byte zone
zoneNo=readByte(3)
log( "Device low batery: " + partNo + "," + zoneNo )
when "822"    #Device low batrery restore 3 byte zone
zoneNo=readByte(3)
log( "Device low batery restore: " + partNo + "," + zoneNo )
when "825"    #Key low batery 3 bytes key 3 bytes zone
zoneNo=readByte(3)
log( "Key low batery: " + partNo + "," + zoneNo )
when "826"    #Key low batery restore 3 bytes zone
zoneNo=readByte(3)
log( "Key low batery restore: " + partNo + "," + zoneNo )
when "827"    #Hnd low batery 3 bytes zone
zoneNo=readByte(3)
log( "Hnd low batery: " + partNo + "," + zoneNo )
when "828"    #Hnd low batery restore 3 bytes zone
zoneNo=readByte(3)
log( "Hnd low batery restore: " + partNo + "," + zoneNo )
when "829"    #General system tamper
log( "General system tamper:" )
when "830"    #General system restore
log( "General system restore:" )
when "831"    #Home automatition trouble
log( "Home automatition trouble:" )
when "832"    #Home automatition trouble restore
log( "Home automatition trouble restore" )
when "840"    #Trouble status 1 byte partition
partNo=readByte(1)
log( "Partition no: " + partNo + "   " )
when "841"    #Trouble status restore 1 byte partition
partNo=readByte(1)
log( "Trouble status restore: " + partNo )
when "842"    #Fire alarm
log( "Fire alarm:" )
when "843"    #Fire alarm restore
log( "Fire alarm restore:" )
when "900"    #Code required    
log( "Code required:" )
else 
log( "Unknown comand for PC5401!!!" )
end

#read check code and end of message
chkCode=readByte(2) 
msgEnd=readByte(2) 
log( " Check sume code: " + chkCode.to_s + "," + "End mess: " )
msgEnd.each_byte { |i| log( i.to_s + "  " ) } 
log( "\n\n" )

#notification try to send another command from buffer
if (cmdNo>= 500) and (cmdNo<=502) then sendCmd2() end
#arm maybe ....

if $bFlush then $logFile.flush end

