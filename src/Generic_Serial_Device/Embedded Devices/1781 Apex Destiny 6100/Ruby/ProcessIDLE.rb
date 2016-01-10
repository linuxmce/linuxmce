#ProcessIDLE 2006-10-10 21:34 ApexDestiny 6100
#log parameters
myTime = Time.now
timeStr = myTime.strftime("%d-%m-%Y  %H:%M:%S  ")
diffTime = myTime - $lastCmdTime

if (  diffTime > 25 ) then
	if( $cmdBuffer.size() > 0 ) then
		log( "No answer from previous command" )
		log( "Start:" + $lastCmdTime.strftime("%d-%m-%Y  %H:%M:%S  ") + "Finish:" + timeStr + "\n" )
		# try another command
		sendCmd2()
	end
end
