#21 April 15:53  Set Level
#$SoundLevel = 0
log( "Set Volume method\n" )

cmd="MV"
head=level[0..0];

case head
when "+" 
	level=level[1..level.size()-1]
	log( "add  " + level.to_i.to_s + "\n" )
	$SoundLevel = $SoundLevel + level.to_i
	log( $SoundLevel.to_s + "\n" )
when "-" 
	level=level[1..level.size()-1]
	$SoundLevel = $SoundLevel - level.to_i
else
	log( "eq  " + level.to_i.to_s + "\n" )
	$SoundLevel = level.to_i
end

if( $SoundLevel > 100 ) then $SoundLevel = 100 end
if( $SoundLevel < 0 ) then $SoundLevel = 0 end

cmd+= "%02d" % $SoundLevel
cmd << "\r"
conn_.Send( cmd )

log( "Set volume AVC" + "  " )
log( "Relative level:" + level.to_s + "  " )
log( "Sound level:" + $SoundLevel.to_s + "  " )
log( "Command:" + cmd + "\n" )
