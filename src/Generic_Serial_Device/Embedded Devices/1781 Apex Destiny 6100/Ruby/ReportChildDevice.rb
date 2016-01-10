def ReportChildDevice
#ReportChildDevice 09-Ian-06 11:00
log( "ReportChildDevice\n" )

#$panelStatus=1	#??
DSCSetTimeStamp(true)
DSCSetTimeBroadcast(true)

if ($bInit == true) then 
	log( "Already intit. Do nothing" + "\n" )
else
	$sensorStatus=Hash.new
	$sensorType=Hash.new
	$partStatus=Hash.new

	$panelStatus=2

	#DSCSetDescArm($bDescArm)
	DSCStatusReport2()
end

end