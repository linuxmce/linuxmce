#private 06 17:17 Denon 28 April 

def TestPrivate()
print "In TestPrivate\n"
end

def log(word)
logTime = Time.now
timeStr = logTime.strftime("%d-%m-%Y  %H:%M:%S  ")

print( timeStr + " " + word)
if( $logFile.nil? == false ) then
	$logFile.print( timeStr + " " + word )
	$logFile.flush()
end
end

def readLine()
	$line=String.new
	$index=0
	
	cod=conn_.Recv(1, $waitTime)
	$line+=cod
while (cod[0,1] != "\r")   do
	if (conn_ != nil) then 
		cod=conn_.Recv(1, $waitTime)
		$line += cod
	else
		log( "Can not read from serial " + "\n" )
	end
	
	if( $line.size() >2 ) then
		log( "Temp received:" + $line + "\n" )
	end
end	
	log( "Final received:" + $line + "\n" )
	return $line
end
