#event  31 A 06 14:50 Denon AVC 
buff = readLine()
cmd=buff[0..1]
param=buff[2..buff.size()-1]

log( "AVR Received  " )
log( "Cmd:" + cmd.to_s + "   " + )
log( "Param:" + param )

case cmd

	when "SI"
	log( "It's an input: command" + param.to_s + "\n" )
	log( "Input sources:" + $inputSources[param.to_s]  + "\n" )
	end # case

	if( param.size() !=0 ) then
		log( "Changing to input " +  param + "\n" )
		cmd = Command.new(device_.devid_, -1001, 1, 2, 49);
		cmd.params_[41] = $inputSources[param.to_s].to_s
		SendCommand(cmd);
	end
	
	when "PW"
	log( "It's a power command:" + param.to_s + "\n" )
		if( param =="STANDBY" ) then
			log( "Turning off:", param , "\n" )
			cmd = Command.new(device_.devid_, -1001, 1, 2, 48);
			cmd.params_[10] = "0";
			SendCommand(cmd);
		else 
			if ( param =="ON" ) then
				plog( "Turning on\n" )
				cmd = Command.new(device_.devid_, -1001, 1, 2, 48);
				cmd.params_[10] = "1";
				SendCommand(cmd);
			end
		end
		
	when "MV"
		newlevel = buff[2..buff.size()-1]
	
		log( "It is a volume command" + param.to_s + "\n" );
		cmd = Command.new(device_.devid_, -1001, 1, 1, 3);
		cmd.params_[158] = newlevel.to_s;
		SendCommand(cmd);
	else
		log( "Unknown comman")
 end   #case
 
 