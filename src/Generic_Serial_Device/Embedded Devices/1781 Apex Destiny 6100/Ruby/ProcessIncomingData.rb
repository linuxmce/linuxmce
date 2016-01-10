#ProcessIncomingData 2006-10-10 21:34 ApexDestiny 6100
#read event code from serial
readLine()

# process the contents of $line, if it got set (maybe readLine should just return it locally)
if( !$line.empty? )
	cmd = $line[0..1]
	log("Command = " + cmd)
	case cmd
		when "LR"
		log("Location Read: " + $line)
		if( "LR200183" == $line[0..7] )
			TurnAllTriggersOnStep2(32, $line[8..$line.length-1])
		elsif( "LR1101A3" == $line[0..7] )
			TurnAllTriggersOnStep2(17, $line[8..$line.length-1])
			# Enable the trigger events over the serial connection
			TurnAllTriggersOnStep3()
		else
		end
		
		when "NQ"
		log("Event notification: " + $line[2..$line.length-1])
		EventHandler($line[2..$line.length-1])
		
		when "AS"
		log("Arming Status: " + $line)
		
		when "ZS"
		log("Zone Status: " + $line)
		
		when "ZP"
		log("Zone Partition: " + $line)
		
	else
		log("unknown answer: " + $line)
	end
else
	log("Error: empty line")
end

sendCmd2()
