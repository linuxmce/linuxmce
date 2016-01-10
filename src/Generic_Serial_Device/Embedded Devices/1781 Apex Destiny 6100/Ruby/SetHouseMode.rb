# SetHouseMode 2006-10-10 22:48 ApexDestiny 6100

auth_user = String.new
if( pk_users.to_i == 0 ) then
	auth_user = "01"
else
	auth_user = "%02d" % pk_users.to_i
end

log( "Value:"         +   value_to_assign           + "  " )
log( "User:"           +   auth_user                    + "  " )
log( "Password:"     +  password                    + "  " )

case value_to_assign.to_i
when 1      #unarmed at home
	ApexDisarmPartition(auth_user, password) 
when 2      # armed away
	ApexArmPartition("AWAY", auth_user, password) 
when 3      # armed at home
	ApexArmPartition("HOME", auth_user, password) 
when 4      # sleeping
	log( "Case 4:\n" )
when 5      # entertaining
	log( "Case 5:\n" )
when 6      # armed extended away
	ApexArmPartition("AWAY", auth_user, password)
end
