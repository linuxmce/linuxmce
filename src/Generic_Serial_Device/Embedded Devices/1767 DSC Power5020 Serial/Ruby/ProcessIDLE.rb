#ProcessIDLE  09-Jun-06 11:40  ApexDestiny 6100
#log parameters
myTime = Time.now
timeStr = myTime.strftime("%d-%m-%Y  %H:%M:%S  ")
diffTime = myTime - $lastCmdTime

if (  diffTime > 25 ) then
	if( $cmdBuffer.size() > 0 ) then
	log( "No answer from previous command" )
	log( "Start:" + $lastCmdTime.strftime("%d-%m-%Y  %H:%M:%S  ") + "Finish:" + timeStr + "\n" )
	
	#sendCmd2()
	end
	#$cmdBuffer=Array.new
end
