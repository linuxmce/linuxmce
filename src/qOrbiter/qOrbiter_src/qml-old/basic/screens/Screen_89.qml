// UI1 Screen GenericAppController (89)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/blankC.png"
		width: 1920
		height: 1080
	}
	function onActivate1784() 
	{
	 // Calling Command 4 (Go back) with CommandGroup_Parameters from 2506
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 4 ")
	}
	ButtonSq {
		id: button1784_1046
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/back.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1784()
	}
	function onActivate1785() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17174
		setCurrentScreen("Screen_1.qml")
	}
	StyledText {
		x: 1680
		y: 0
		height: 1
		width: 1
		textLabel: "Main menu"
	}
	ButtonSq {
		id: button1785_1047
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate3557() 
	{
	}
	ButtonSq {
		id: button3557_1048
		x: 1680
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3557()
	}
	ButtonSq {
		id: button3572_1049
		x: 0
		y: 0
		width: 960
		height: 720
		buttonbackground: "skins/Basic/Backgrounds/mousepad.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3572()
	}
	function onActivate3573() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15172
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-256\" 12 \"-256\" ")
	}
	function onActivate3574() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15173
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-256\" 12 \"-128\" ")
	}
	function onActivate3575() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15174
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-256\" 12 \"-64\" ")
	}
	function onActivate3576() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15175
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-256\" 12 \"-32\" ")
	}
	function onActivate3577() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15176
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-256\" 12 \"-16\" ")
	}
	function onActivate3578() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15177
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-256\" 12 \"-8\" ")
	}
	function onActivate3579() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15178
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-256\" 12 \"-4\" ")
	}
	function onActivate3580() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15179
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-256\" 12 \"-2\" ")
	}
	function onActivate3581() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15180
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-256\" 12 \"2\" ")
	}
	function onActivate3582() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15181
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-256\" 12 \"4\" ")
	}
	function onActivate3583() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15182
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-256\" 12 \"8\" ")
	}
	function onActivate3584() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15183
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-256\" 12 \"16\" ")
	}
	function onActivate3585() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15184
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-256\" 12 \"32\" ")
	}
	function onActivate3586() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15185
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-256\" 12 \"64\" ")
	}
	function onActivate3587() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15186
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-256\" 12 \"128\" ")
	}
	function onActivate3588() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15187
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-256\" 12 \"256\" ")
	}
	function onActivate3589() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15188
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-128\" 12 \"-256\" ")
	}
	function onActivate3590() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15189
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-128\" 12 \"-128\" ")
	}
	function onActivate3591() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15190
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-128\" 12 \"-64\" ")
	}
	function onActivate3592() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15191
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-128\" 12 \"-32\" ")
	}
	function onActivate3593() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15192
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-128\" 12 \"-16\" ")
	}
	function onActivate3594() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15193
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-128\" 12 \"-8\" ")
	}
	function onActivate3595() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15194
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-128\" 12 \"-4\" ")
	}
	function onActivate3596() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15195
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-128\" 12 \"-2\" ")
	}
	function onActivate3597() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15196
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-128\" 12 \"2\" ")
	}
	function onActivate3598() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15197
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-128\" 12 \"4\" ")
	}
	function onActivate3599() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15198
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-128\" 12 \"8\" ")
	}
	function onActivate3600() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15199
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-128\" 12 \"16\" ")
	}
	function onActivate3601() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15200
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-128\" 12 \"32\" ")
	}
	function onActivate3602() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15201
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-128\" 12 \"64\" ")
	}
	function onActivate3603() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15202
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-128\" 12 \"128\" ")
	}
	function onActivate3604() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15203
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-128\" 12 \"256\" ")
	}
	function onActivate3605() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15204
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-64\" 12 \"-256\" ")
	}
	function onActivate3606() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15205
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-64\" 12 \"-128\" ")
	}
	function onActivate3607() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15206
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-64\" 12 \"-64\" ")
	}
	function onActivate3608() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15207
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-64\" 12 \"-32\" ")
	}
	function onActivate3609() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15208
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-64\" 12 \"-16\" ")
	}
	function onActivate3610() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15209
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-64\" 12 \"-8\" ")
	}
	function onActivate3611() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15210
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-64\" 12 \"-4\" ")
	}
	function onActivate3612() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15211
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-64\" 12 \"-2\" ")
	}
	function onActivate3613() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15212
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-64\" 12 \"2\" ")
	}
	function onActivate3614() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15213
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-64\" 12 \"4\" ")
	}
	function onActivate3615() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15214
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-64\" 12 \"8\" ")
	}
	function onActivate3616() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15215
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-64\" 12 \"16\" ")
	}
	function onActivate3617() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15216
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-64\" 12 \"32\" ")
	}
	function onActivate3618() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15217
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-64\" 12 \"64\" ")
	}
	function onActivate3619() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15218
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-64\" 12 \"128\" ")
	}
	function onActivate3620() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15219
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-64\" 12 \"256\" ")
	}
	function onActivate3621() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15220
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-32\" 12 \"-256\" ")
	}
	function onActivate3622() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15221
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-32\" 12 \"-128\" ")
	}
	function onActivate3623() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15222
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-32\" 12 \"-64\" ")
	}
	function onActivate3624() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15223
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-32\" 12 \"-32\" ")
	}
	function onActivate3625() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15224
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-32\" 12 \"-16\" ")
	}
	function onActivate3626() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15225
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-32\" 12 \"-8\" ")
	}
	function onActivate3627() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15226
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-32\" 12 \"-4\" ")
	}
	function onActivate3628() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15227
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-32\" 12 \"-2\" ")
	}
	function onActivate3629() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15228
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-32\" 12 \"2\" ")
	}
	function onActivate3630() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15229
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-32\" 12 \"4\" ")
	}
	function onActivate3631() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15230
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-32\" 12 \"8\" ")
	}
	function onActivate3632() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15231
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-32\" 12 \"16\" ")
	}
	function onActivate3633() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15232
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-32\" 12 \"32\" ")
	}
	function onActivate3634() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15233
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-32\" 12 \"64\" ")
	}
	function onActivate3635() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15234
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-32\" 12 \"128\" ")
	}
	function onActivate3636() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15235
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-32\" 12 \"256\" ")
	}
	function onActivate3637() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15236
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-16\" 12 \"-256\" ")
	}
	function onActivate3638() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15237
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-16\" 12 \"-128\" ")
	}
	function onActivate3639() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15238
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-16\" 12 \"-64\" ")
	}
	function onActivate3640() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15239
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-16\" 12 \"-32\" ")
	}
	function onActivate3641() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15240
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-16\" 12 \"-16\" ")
	}
	function onActivate3642() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15241
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-16\" 12 \"-8\" ")
	}
	function onActivate3643() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15242
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-16\" 12 \"-4\" ")
	}
	function onActivate3644() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15243
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-16\" 12 \"-2\" ")
	}
	function onActivate3645() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15244
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-16\" 12 \"2\" ")
	}
	function onActivate3646() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15245
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-16\" 12 \"4\" ")
	}
	function onActivate3647() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15246
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-16\" 12 \"8\" ")
	}
	function onActivate3648() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15247
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-16\" 12 \"16\" ")
	}
	function onActivate3649() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15248
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-16\" 12 \"32\" ")
	}
	function onActivate3650() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15249
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-16\" 12 \"64\" ")
	}
	function onActivate3651() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15250
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-16\" 12 \"128\" ")
	}
	function onActivate3652() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15251
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-16\" 12 \"256\" ")
	}
	function onActivate3653() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15252
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-8\" 12 \"-256\" ")
	}
	function onActivate3654() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15253
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-8\" 12 \"-128\" ")
	}
	function onActivate3655() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15254
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-8\" 12 \"-64\" ")
	}
	function onActivate3656() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15255
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-8\" 12 \"-32\" ")
	}
	function onActivate3657() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15256
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-8\" 12 \"-16\" ")
	}
	function onActivate3658() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15257
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-8\" 12 \"-8\" ")
	}
	function onActivate3659() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15258
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-8\" 12 \"-4\" ")
	}
	function onActivate3660() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15259
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-8\" 12 \"-2\" ")
	}
	function onActivate3661() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15260
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-8\" 12 \"2\" ")
	}
	function onActivate3662() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15261
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-8\" 12 \"4\" ")
	}
	function onActivate3663() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15262
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-8\" 12 \"8\" ")
	}
	function onActivate3664() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15263
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-8\" 12 \"16\" ")
	}
	function onActivate3665() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15264
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-8\" 12 \"32\" ")
	}
	function onActivate3666() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15265
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-8\" 12 \"64\" ")
	}
	function onActivate3667() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15266
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-8\" 12 \"128\" ")
	}
	function onActivate3668() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15267
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-8\" 12 \"256\" ")
	}
	function onActivate3669() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15268
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-4\" 12 \"-256\" ")
	}
	function onActivate3670() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15269
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-4\" 12 \"-128\" ")
	}
	function onActivate3671() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15270
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-4\" 12 \"-64\" ")
	}
	function onActivate3672() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15271
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-4\" 12 \"-32\" ")
	}
	function onActivate3673() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15272
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-4\" 12 \"-16\" ")
	}
	function onActivate3674() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15273
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-4\" 12 \"-8\" ")
	}
	function onActivate3675() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15274
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-4\" 12 \"-4\" ")
	}
	function onActivate3676() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15275
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-4\" 12 \"-2\" ")
	}
	function onActivate3677() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15276
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-4\" 12 \"2\" ")
	}
	function onActivate3678() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15277
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-4\" 12 \"4\" ")
	}
	function onActivate3679() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15278
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-4\" 12 \"8\" ")
	}
	function onActivate3680() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15279
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-4\" 12 \"16\" ")
	}
	function onActivate3681() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15280
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-4\" 12 \"32\" ")
	}
	function onActivate3682() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15281
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-4\" 12 \"64\" ")
	}
	function onActivate3683() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15282
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-4\" 12 \"128\" ")
	}
	function onActivate3684() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15283
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-4\" 12 \"256\" ")
	}
	function onActivate3685() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15284
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-2\" 12 \"-256\" ")
	}
	function onActivate3686() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15285
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-2\" 12 \"-128\" ")
	}
	function onActivate3687() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15286
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-2\" 12 \"-64\" ")
	}
	function onActivate3688() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15287
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-2\" 12 \"-32\" ")
	}
	function onActivate3689() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15288
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-2\" 12 \"-16\" ")
	}
	function onActivate3690() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15289
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-2\" 12 \"-8\" ")
	}
	function onActivate3691() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15290
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-2\" 12 \"-4\" ")
	}
	function onActivate3692() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15291
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-2\" 12 \"-2\" ")
	}
	function onActivate3693() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15292
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-2\" 12 \"2\" ")
	}
	function onActivate3694() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15293
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-2\" 12 \"4\" ")
	}
	function onActivate3695() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15294
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-2\" 12 \"8\" ")
	}
	function onActivate3696() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15295
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-2\" 12 \"16\" ")
	}
	function onActivate3697() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15296
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-2\" 12 \"32\" ")
	}
	function onActivate3698() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15297
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-2\" 12 \"64\" ")
	}
	function onActivate3699() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15298
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-2\" 12 \"128\" ")
	}
	function onActivate3700() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15299
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"-2\" 12 \"256\" ")
	}
	function onActivate3701() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15300
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"2\" 12 \"-256\" ")
	}
	function onActivate3702() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15301
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"2\" 12 \"-128\" ")
	}
	function onActivate3703() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15302
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"2\" 12 \"-64\" ")
	}
	function onActivate3704() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15303
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"2\" 12 \"-32\" ")
	}
	function onActivate3705() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15304
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"2\" 12 \"-16\" ")
	}
	function onActivate3706() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15305
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"2\" 12 \"-8\" ")
	}
	function onActivate3707() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15306
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"2\" 12 \"-4\" ")
	}
	function onActivate3708() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15307
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"2\" 12 \"-2\" ")
	}
	function onActivate3709() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15308
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"2\" 12 \"2\" ")
	}
	function onActivate3710() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15309
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"2\" 12 \"4\" ")
	}
	function onActivate3711() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15310
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"2\" 12 \"8\" ")
	}
	function onActivate3712() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15311
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"2\" 12 \"16\" ")
	}
	function onActivate3713() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15312
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"2\" 12 \"32\" ")
	}
	function onActivate3714() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15313
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"2\" 12 \"64\" ")
	}
	function onActivate3715() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15314
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"2\" 12 \"128\" ")
	}
	function onActivate3716() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15315
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"2\" 12 \"256\" ")
	}
	function onActivate3717() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15316
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"4\" 12 \"-256\" ")
	}
	function onActivate3718() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15317
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"4\" 12 \"-128\" ")
	}
	function onActivate3719() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15318
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"4\" 12 \"-64\" ")
	}
	function onActivate3720() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15319
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"4\" 12 \"-32\" ")
	}
	function onActivate3721() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15320
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"4\" 12 \"-16\" ")
	}
	function onActivate3722() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15321
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"4\" 12 \"-8\" ")
	}
	function onActivate3723() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15322
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"4\" 12 \"-4\" ")
	}
	function onActivate3724() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15323
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"4\" 12 \"-2\" ")
	}
	function onActivate3725() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15324
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"4\" 12 \"2\" ")
	}
	function onActivate3726() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15325
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"4\" 12 \"4\" ")
	}
	function onActivate3727() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15326
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"4\" 12 \"8\" ")
	}
	function onActivate3728() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15327
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"4\" 12 \"16\" ")
	}
	function onActivate3729() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15328
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"4\" 12 \"32\" ")
	}
	function onActivate3730() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15329
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"4\" 12 \"64\" ")
	}
	function onActivate3731() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15330
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"4\" 12 \"128\" ")
	}
	function onActivate3732() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15331
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"4\" 12 \"256\" ")
	}
	function onActivate3733() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15332
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"8\" 12 \"-256\" ")
	}
	function onActivate3734() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15333
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"8\" 12 \"-128\" ")
	}
	function onActivate3735() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15334
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"8\" 12 \"-64\" ")
	}
	function onActivate3736() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15335
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"8\" 12 \"-32\" ")
	}
	function onActivate3737() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15336
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"8\" 12 \"-16\" ")
	}
	function onActivate3738() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15337
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"8\" 12 \"-8\" ")
	}
	function onActivate3739() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15338
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"8\" 12 \"-4\" ")
	}
	function onActivate3740() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15339
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"8\" 12 \"-2\" ")
	}
	function onActivate3741() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15340
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"8\" 12 \"2\" ")
	}
	function onActivate3742() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15341
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"8\" 12 \"4\" ")
	}
	function onActivate3743() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15342
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"8\" 12 \"8\" ")
	}
	function onActivate3744() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15343
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"8\" 12 \"16\" ")
	}
	function onActivate3745() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15344
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"8\" 12 \"32\" ")
	}
	function onActivate3746() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15345
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"8\" 12 \"64\" ")
	}
	function onActivate3747() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15346
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"8\" 12 \"128\" ")
	}
	function onActivate3748() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15347
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"8\" 12 \"256\" ")
	}
	function onActivate3749() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15348
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"16\" 12 \"-256\" ")
	}
	function onActivate3750() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15349
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"16\" 12 \"-128\" ")
	}
	function onActivate3751() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15350
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"16\" 12 \"-64\" ")
	}
	function onActivate3752() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15351
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"16\" 12 \"-32\" ")
	}
	function onActivate3753() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15352
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"16\" 12 \"-16\" ")
	}
	function onActivate3754() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15353
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"16\" 12 \"-8\" ")
	}
	function onActivate3755() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15354
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"16\" 12 \"-4\" ")
	}
	function onActivate3756() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15355
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"16\" 12 \"-2\" ")
	}
	function onActivate3757() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15356
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"16\" 12 \"2\" ")
	}
	function onActivate3758() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15357
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"16\" 12 \"4\" ")
	}
	function onActivate3759() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15358
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"16\" 12 \"8\" ")
	}
	function onActivate3760() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15359
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"16\" 12 \"16\" ")
	}
	function onActivate3761() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15360
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"16\" 12 \"32\" ")
	}
	function onActivate3762() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15361
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"16\" 12 \"64\" ")
	}
	function onActivate3763() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15362
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"16\" 12 \"128\" ")
	}
	function onActivate3764() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15363
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"16\" 12 \"256\" ")
	}
	function onActivate3765() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15364
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"32\" 12 \"-256\" ")
	}
	function onActivate3766() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15365
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"32\" 12 \"-128\" ")
	}
	function onActivate3767() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15366
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"32\" 12 \"-64\" ")
	}
	function onActivate3768() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15367
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"32\" 12 \"-32\" ")
	}
	function onActivate3769() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15368
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"32\" 12 \"-16\" ")
	}
	function onActivate3770() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15369
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"32\" 12 \"-8\" ")
	}
	function onActivate3771() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15370
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"32\" 12 \"-4\" ")
	}
	function onActivate3772() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15371
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"32\" 12 \"-2\" ")
	}
	function onActivate3773() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15372
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"32\" 12 \"2\" ")
	}
	function onActivate3774() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15373
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"32\" 12 \"4\" ")
	}
	function onActivate3775() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15374
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"32\" 12 \"8\" ")
	}
	function onActivate3776() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15375
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"32\" 12 \"16\" ")
	}
	function onActivate3777() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15376
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"32\" 12 \"32\" ")
	}
	function onActivate3778() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15377
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"32\" 12 \"64\" ")
	}
	function onActivate3779() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15378
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"32\" 12 \"128\" ")
	}
	function onActivate3780() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15379
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"32\" 12 \"256\" ")
	}
	function onActivate3781() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15380
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"64\" 12 \"-256\" ")
	}
	function onActivate3782() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15381
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"64\" 12 \"-128\" ")
	}
	function onActivate3783() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15382
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"64\" 12 \"-64\" ")
	}
	function onActivate3784() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15383
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"64\" 12 \"-32\" ")
	}
	function onActivate3785() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15384
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"64\" 12 \"-16\" ")
	}
	function onActivate3786() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15385
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"64\" 12 \"-8\" ")
	}
	function onActivate3787() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15386
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"64\" 12 \"-4\" ")
	}
	function onActivate3788() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15387
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"64\" 12 \"-2\" ")
	}
	function onActivate3789() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15388
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"64\" 12 \"2\" ")
	}
	function onActivate3790() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15389
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"64\" 12 \"4\" ")
	}
	function onActivate3791() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15390
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"64\" 12 \"8\" ")
	}
	function onActivate3792() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15391
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"64\" 12 \"16\" ")
	}
	function onActivate3793() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15392
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"64\" 12 \"32\" ")
	}
	function onActivate3794() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15393
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"64\" 12 \"64\" ")
	}
	function onActivate3795() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15394
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"64\" 12 \"128\" ")
	}
	function onActivate3796() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15395
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"64\" 12 \"256\" ")
	}
	function onActivate3797() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15396
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"128\" 12 \"-256\" ")
	}
	function onActivate3798() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15397
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"128\" 12 \"-128\" ")
	}
	function onActivate3799() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15398
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"128\" 12 \"-64\" ")
	}
	function onActivate3800() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15399
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"128\" 12 \"-32\" ")
	}
	function onActivate3801() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15400
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"128\" 12 \"-16\" ")
	}
	function onActivate3802() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15401
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"128\" 12 \"-8\" ")
	}
	function onActivate3803() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15402
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"128\" 12 \"-4\" ")
	}
	function onActivate3804() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15403
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"128\" 12 \"-2\" ")
	}
	function onActivate3805() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15404
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"128\" 12 \"2\" ")
	}
	function onActivate3806() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15405
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"128\" 12 \"4\" ")
	}
	function onActivate3807() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15406
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"128\" 12 \"8\" ")
	}
	function onActivate3808() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15407
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"128\" 12 \"16\" ")
	}
	function onActivate3809() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15408
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"128\" 12 \"32\" ")
	}
	function onActivate3810() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15409
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"128\" 12 \"64\" ")
	}
	function onActivate3811() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15410
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"128\" 12 \"128\" ")
	}
	function onActivate3812() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15411
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"128\" 12 \"256\" ")
	}
	function onActivate3813() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15412
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"256\" 12 \"-256\" ")
	}
	function onActivate3814() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15413
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"256\" 12 \"-128\" ")
	}
	function onActivate3815() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15414
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"256\" 12 \"-64\" ")
	}
	function onActivate3816() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15415
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"256\" 12 \"-32\" ")
	}
	function onActivate3817() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15416
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"256\" 12 \"-16\" ")
	}
	function onActivate3818() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15417
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"256\" 12 \"-8\" ")
	}
	function onActivate3819() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15418
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"256\" 12 \"-4\" ")
	}
	function onActivate3820() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15419
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"256\" 12 \"-2\" ")
	}
	function onActivate3821() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15420
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"256\" 12 \"2\" ")
	}
	function onActivate3822() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15421
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"256\" 12 \"4\" ")
	}
	function onActivate3823() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15422
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"256\" 12 \"8\" ")
	}
	function onActivate3824() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15423
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"256\" 12 \"16\" ")
	}
	function onActivate3825() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15424
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"256\" 12 \"32\" ")
	}
	function onActivate3826() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15425
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"256\" 12 \"64\" ")
	}
	function onActivate3827() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15426
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"256\" 12 \"128\" ")
	}
	function onActivate3828() 
	{
	 // Calling Command 415 (Set Mouse Position Relative) with CommandGroup_Parameters from 15427
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 415 11 \"256\" 12 \"256\" ")
	}
	function onActivate3829() 
	{
	 // Calling Command 416 (Simulate Mouse Click At Present Pos) with CommandGroup_Parameters from 15429
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 416 14 \"L\" ")
	}
	StyledText {
		x: 960
		y: 360
		height: 240
		width: 180
		textLabel: "Left"
	}
	ButtonSq {
		id: button3829_1050
		x: 960
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Left"
		clickHandler.onClicked: onActivate3829()
	}
	function onActivate3830() 
	{
	 // Calling Command 416 (Simulate Mouse Click At Present Pos) with CommandGroup_Parameters from 15430
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 416 14 \"R\" ")
	}
	StyledText {
		x: 1200
		y: 360
		height: 240
		width: 180
		textLabel: "Right"
	}
	ButtonSq {
		id: button3830_1051
		x: 1200
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/square.png"
		buttontext: "Right"
		clickHandler.onClicked: onActivate3830()
	}
	function onActivate2491() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13813
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"25\" ")
	}
	StyledText {
		x: 1200
		y: 1445
		height: 120
		width: 90
		textLabel: ")
0"
	}
	ButtonSq {
		id: button2491_1052
		x: 1200
		y: 1445
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ")
0"
		clickHandler.onClicked: onActivate2491()
	}
	function onActivate2482() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13814
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"16\" ")
	}
	StyledText {
		x: 120
		y: 1445
		height: 120
		width: 90
		textLabel: "!
1"
	}
	ButtonSq {
		id: button2482_1053
		x: 120
		y: 1445
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "!
1"
		clickHandler.onClicked: onActivate2482()
	}
	function onActivate2483() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13815
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"17\" ")
	}
	StyledText {
		x: 240
		y: 1445
		height: 120
		width: 90
		textLabel: "@
2"
	}
	ButtonSq {
		id: button2483_1054
		x: 240
		y: 1445
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "@
2"
		clickHandler.onClicked: onActivate2483()
	}
	function onActivate2484() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13816
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"18\" ")
	}
	StyledText {
		x: 360
		y: 1445
		height: 120
		width: 90
		textLabel: "#
3"
	}
	ButtonSq {
		id: button2484_1055
		x: 360
		y: 1445
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "#
3"
		clickHandler.onClicked: onActivate2484()
	}
	function onActivate2485() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13817
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"19\" ")
	}
	StyledText {
		x: 480
		y: 1445
		height: 120
		width: 90
		textLabel: "$
4"
	}
	ButtonSq {
		id: button2485_1056
		x: 480
		y: 1445
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "$
4"
		clickHandler.onClicked: onActivate2485()
	}
	function onActivate2486() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13818
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"20\" ")
	}
	StyledText {
		x: 600
		y: 1445
		height: 120
		width: 90
		textLabel: "%
5"
	}
	ButtonSq {
		id: button2486_1057
		x: 600
		y: 1445
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "%
5"
		clickHandler.onClicked: onActivate2486()
	}
	function onActivate2487() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13819
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"21\" ")
	}
	StyledText {
		x: 720
		y: 1445
		height: 120
		width: 90
		textLabel: "^
6"
	}
	ButtonSq {
		id: button2487_1058
		x: 720
		y: 1445
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "^
6"
		clickHandler.onClicked: onActivate2487()
	}
	function onActivate2488() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13820
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"22\" ")
	}
	StyledText {
		x: 840
		y: 1445
		height: 120
		width: 90
		textLabel: "&
7"
	}
	ButtonSq {
		id: button2488_1059
		x: 840
		y: 1445
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "&
7"
		clickHandler.onClicked: onActivate2488()
	}
	function onActivate2489() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13821
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"23\" ")
	}
	StyledText {
		x: 960
		y: 1445
		height: 120
		width: 90
		textLabel: "*
8"
	}
	ButtonSq {
		id: button2489_1060
		x: 960
		y: 1445
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "*
8"
		clickHandler.onClicked: onActivate2489()
	}
	function onActivate2490() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13822
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"24\" ")
	}
	StyledText {
		x: 1080
		y: 1445
		height: 120
		width: 90
		textLabel: "(
9"
	}
	ButtonSq {
		id: button2490_1061
		x: 1080
		y: 1445
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "(
9"
		clickHandler.onClicked: onActivate2490()
	}
	function onActivate2492() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13823
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"182\" 50 \"dash\" ")
	}
	StyledText {
		x: 1320
		y: 1445
		height: 120
		width: 90
		textLabel: "_
-"
	}
	ButtonSq {
		id: button2492_1062
		x: 1320
		y: 1445
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "_
-"
		clickHandler.onClicked: onActivate2492()
	}
	function onActivate2493() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13824
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"197\" 50 \"equals sign\" ")
	}
	StyledText {
		x: 1440
		y: 1445
		height: 120
		width: 90
		textLabel: "+
="
	}
	ButtonSq {
		id: button2493_1063
		x: 1440
		y: 1445
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "+
="
		clickHandler.onClicked: onActivate2493()
	}
	function onActivate2510() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13825
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"129\" ")
	}
	StyledText {
		x: 180
		y: 1535
		height: 120
		width: 90
		textLabel: "Q"
	}
	ButtonSq {
		id: button2510_1064
		x: 180
		y: 1535
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "Q"
		clickHandler.onClicked: onActivate2510()
	}
	function onActivate2516() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13826
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"135\" ")
	}
	StyledText {
		x: 300
		y: 1535
		height: 120
		width: 90
		textLabel: "W"
	}
	ButtonSq {
		id: button2516_1065
		x: 300
		y: 1535
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "W"
		clickHandler.onClicked: onActivate2516()
	}
	function onActivate2498() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13827
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"117\" ")
	}
	StyledText {
		x: 420
		y: 1535
		height: 120
		width: 90
		textLabel: "E"
	}
	ButtonSq {
		id: button2498_1066
		x: 420
		y: 1535
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "E"
		clickHandler.onClicked: onActivate2498()
	}
	function onActivate2511() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13828
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"130\" ")
	}
	StyledText {
		x: 540
		y: 1535
		height: 120
		width: 90
		textLabel: "R"
	}
	ButtonSq {
		id: button2511_1067
		x: 540
		y: 1535
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "R"
		clickHandler.onClicked: onActivate2511()
	}
	function onActivate2513() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13829
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"132\" ")
	}
	StyledText {
		x: 660
		y: 1535
		height: 120
		width: 90
		textLabel: "T"
	}
	ButtonSq {
		id: button2513_1068
		x: 660
		y: 1535
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "T"
		clickHandler.onClicked: onActivate2513()
	}
	function onActivate2518() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13830
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"137\" ")
	}
	StyledText {
		x: 780
		y: 1535
		height: 120
		width: 90
		textLabel: "Y"
	}
	ButtonSq {
		id: button2518_1069
		x: 780
		y: 1535
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "Y"
		clickHandler.onClicked: onActivate2518()
	}
	function onActivate2514() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13831
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"133\" ")
	}
	StyledText {
		x: 900
		y: 1535
		height: 120
		width: 90
		textLabel: "U"
	}
	ButtonSq {
		id: button2514_1070
		x: 900
		y: 1535
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "U"
		clickHandler.onClicked: onActivate2514()
	}
	function onActivate2502() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13832
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"121\" ")
	}
	StyledText {
		x: 1020
		y: 1535
		height: 120
		width: 90
		textLabel: "I"
	}
	ButtonSq {
		id: button2502_1071
		x: 1020
		y: 1535
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "I"
		clickHandler.onClicked: onActivate2502()
	}
	function onActivate2508() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13833
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"127\" ")
	}
	StyledText {
		x: 1140
		y: 1535
		height: 120
		width: 90
		textLabel: "O"
	}
	ButtonSq {
		id: button2508_1072
		x: 1140
		y: 1535
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "O"
		clickHandler.onClicked: onActivate2508()
	}
	function onActivate2509() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13834
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"128\" ")
	}
	StyledText {
		x: 1260
		y: 1535
		height: 120
		width: 90
		textLabel: "P"
	}
	ButtonSq {
		id: button2509_1073
		x: 1260
		y: 1535
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "P"
		clickHandler.onClicked: onActivate2509()
	}
	function onActivate2494() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13835
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"113\" ")
	}
	StyledText {
		x: 240
		y: 1625
		height: 120
		width: 90
		textLabel: "A"
	}
	ButtonSq {
		id: button2494_1074
		x: 240
		y: 1625
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "A"
		clickHandler.onClicked: onActivate2494()
	}
	function onActivate2512() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13836
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"131\" ")
	}
	StyledText {
		x: 360
		y: 1625
		height: 120
		width: 90
		textLabel: "S"
	}
	ButtonSq {
		id: button2512_1075
		x: 360
		y: 1625
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "S"
		clickHandler.onClicked: onActivate2512()
	}
	function onActivate2497() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13837
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"116\" ")
	}
	StyledText {
		x: 480
		y: 1625
		height: 120
		width: 90
		textLabel: "D"
	}
	ButtonSq {
		id: button2497_1076
		x: 480
		y: 1625
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "D"
		clickHandler.onClicked: onActivate2497()
	}
	function onActivate2499() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13838
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"118\" ")
	}
	StyledText {
		x: 600
		y: 1625
		height: 120
		width: 90
		textLabel: "F"
	}
	ButtonSq {
		id: button2499_1077
		x: 600
		y: 1625
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "F"
		clickHandler.onClicked: onActivate2499()
	}
	function onActivate2500() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13839
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"119\" ")
	}
	StyledText {
		x: 720
		y: 1625
		height: 120
		width: 90
		textLabel: "G"
	}
	ButtonSq {
		id: button2500_1078
		x: 720
		y: 1625
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "G"
		clickHandler.onClicked: onActivate2500()
	}
	function onActivate2501() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13840
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"120\" ")
	}
	StyledText {
		x: 840
		y: 1625
		height: 120
		width: 90
		textLabel: "H"
	}
	ButtonSq {
		id: button2501_1079
		x: 840
		y: 1625
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "H"
		clickHandler.onClicked: onActivate2501()
	}
	function onActivate2503() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13841
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"122\" ")
	}
	StyledText {
		x: 960
		y: 1625
		height: 120
		width: 90
		textLabel: "J"
	}
	ButtonSq {
		id: button2503_1080
		x: 960
		y: 1625
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "J"
		clickHandler.onClicked: onActivate2503()
	}
	function onActivate2504() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13842
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"123\" ")
	}
	StyledText {
		x: 1080
		y: 1625
		height: 120
		width: 90
		textLabel: "K"
	}
	ButtonSq {
		id: button2504_1081
		x: 1080
		y: 1625
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "K"
		clickHandler.onClicked: onActivate2504()
	}
	function onActivate2505() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13843
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"124\" ")
	}
	StyledText {
		x: 1200
		y: 1625
		height: 120
		width: 90
		textLabel: "L"
	}
	ButtonSq {
		id: button2505_1082
		x: 1200
		y: 1625
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "L"
		clickHandler.onClicked: onActivate2505()
	}
	function onActivate2519() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13844
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"138\" ")
	}
	StyledText {
		x: 300
		y: 1715
		height: 120
		width: 90
		textLabel: "Z"
	}
	ButtonSq {
		id: button2519_1083
		x: 300
		y: 1715
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "Z"
		clickHandler.onClicked: onActivate2519()
	}
	function onActivate2517() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13845
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"136\" ")
	}
	StyledText {
		x: 420
		y: 1715
		height: 120
		width: 90
		textLabel: "X"
	}
	ButtonSq {
		id: button2517_1084
		x: 420
		y: 1715
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "X"
		clickHandler.onClicked: onActivate2517()
	}
	function onActivate2496() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13846
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"115\" ")
	}
	StyledText {
		x: 540
		y: 1715
		height: 120
		width: 90
		textLabel: "C"
	}
	ButtonSq {
		id: button2496_1085
		x: 540
		y: 1715
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "C"
		clickHandler.onClicked: onActivate2496()
	}
	function onActivate2515() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13847
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"134\" ")
	}
	StyledText {
		x: 660
		y: 1715
		height: 120
		width: 90
		textLabel: "V"
	}
	ButtonSq {
		id: button2515_1086
		x: 660
		y: 1715
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "V"
		clickHandler.onClicked: onActivate2515()
	}
	function onActivate2495() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13848
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"114\" ")
	}
	StyledText {
		x: 780
		y: 1715
		height: 120
		width: 90
		textLabel: "B"
	}
	ButtonSq {
		id: button2495_1087
		x: 780
		y: 1715
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "B"
		clickHandler.onClicked: onActivate2495()
	}
	function onActivate2507() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13849
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"126\" ")
	}
	StyledText {
		x: 900
		y: 1715
		height: 120
		width: 90
		textLabel: "N"
	}
	ButtonSq {
		id: button2507_1088
		x: 900
		y: 1715
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "N"
		clickHandler.onClicked: onActivate2507()
	}
	function onActivate2506() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13850
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"125\" ")
	}
	StyledText {
		x: 1020
		y: 1715
		height: 120
		width: 90
		textLabel: "M"
	}
	ButtonSq {
		id: button2506_1089
		x: 1020
		y: 1715
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "M"
		clickHandler.onClicked: onActivate2506()
	}
	function onActivate2522() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13851
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"184\" 50 \";\" ")
	}
	StyledText {
		x: 1320
		y: 1625
		height: 120
		width: 90
		textLabel: ":
;"
	}
	ButtonSq {
		id: button2522_1090
		x: 1320
		y: 1625
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ":
;"
		clickHandler.onClicked: onActivate2522()
	}
	function onActivate2523() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13852
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"185\" 50 \"\'\" ")
	}
	StyledText {
		x: 1440
		y: 1625
		height: 120
		width: 90
		textLabel: ""

	}
	ButtonSq {
		id: button2523_1091
		x: 1440
		y: 1625
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ""

		clickHandler.onClicked: onActivate2523()
	}
	function onActivate2524() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13853
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"166\" ")
	}
	StyledText {
		x: 1140
		y: 1715
		height: 120
		width: 90
		textLabel: ","
	}
	ButtonSq {
		id: button2524_1092
		x: 1140
		y: 1715
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ","
		clickHandler.onClicked: onActivate2524()
	}
	function onActivate2525() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13854
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"186\" 50 \".\" ")
	}
	StyledText {
		x: 1260
		y: 1715
		height: 120
		width: 90
		textLabel: "."
	}
	ButtonSq {
		id: button2525_1093
		x: 1260
		y: 1715
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "."
		clickHandler.onClicked: onActivate2525()
	}
	function onActivate2528() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13855
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"112\" ")
	}
	StyledText {
		x: 1380
		y: 1535
		height: 120
		width: 135
		textLabel: "<- Back"
	}
	ButtonSq {
		id: button2528_1094
		x: 1380
		y: 1535
		width: 180
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "<- Back"
		clickHandler.onClicked: onActivate2528()
	}
	function onActivate2520() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13856
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"165\" ")
	}
	StyledText {
		x: 1380
		y: 1715
		height: 120
		width: 135
		textLabel: "Space"
	}
	ButtonSq {
		id: button2520_1095
		x: 1380
		y: 1715
		width: 180
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "Space"
		clickHandler.onClicked: onActivate2520()
	}
	function onActivate2527() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13857
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"168\" ")
	}
	StyledText {
		x: 120
		y: 1715
		height: 120
		width: 135
		textLabel: "Shift"
	}
	ButtonSq {
		id: button2527_1096
		x: 120
		y: 1715
		width: 180
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "Shift"
		clickHandler.onClicked: onActivate2527()
	}
	function onActivate2529() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 13858
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"170\" ")
	}
	StyledText {
		x: 120
		y: 1625
		height: 120
		width: 90
		textLabel: "cap
lock"
	}
	ButtonSq {
		id: button2529_1097
		x: 120
		y: 1625
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "cap
lock"
		clickHandler.onClicked: onActivate2529()
	}
	function onActivate4305() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16023
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"187\" ")
	}
	StyledText {
		x: 0
		y: 723
		height: 120
		width: 90
		textLabel: "Esc"
	}
	ButtonSq {
		id: button4305_1098
		x: 0
		y: 723
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: "Esc"
		clickHandler.onClicked: onActivate4305()
	}
	function onActivate4306() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16024
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"188\" ")
	}
	StyledText {
		x: 0
		y: 813
		height: 120
		width: 135
		textLabel: "Tab"
	}
	ButtonSq {
		id: button4306_1099
		x: 0
		y: 813
		width: 180
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "Tab"
		clickHandler.onClicked: onActivate4306()
	}
	function onActivate4312() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16025
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 28 26 \"5\" ")
	}
	StyledText {
		x: 1652
		y: 813
		height: 120
		width: 135
		textLabel: "Enter"
	}
	ButtonSq {
		id: button4312_1100
		x: 1652
		y: 813
		width: 180
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20Wide%20KB.png"
		buttontext: "Enter"
		clickHandler.onClicked: onActivate4312()
	}
	function onActivate4310() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16028
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 28 26 \"3\" ")
	}
	ButtonSq {
		id: button4310_1101
		x: 1560
		y: 993
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4310()
	}
	ButtonSq {
		id: button4315_1102
		x: 1560
		y: 1715
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/left.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4315()
	}
	function onActivate4308() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16026
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 28 26 \"1\" ")
	}
	ButtonSq {
		id: button4308_1103
		x: 1680
		y: 903
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4308()
	}
	ButtonSq {
		id: button4313_1104
		x: 1680
		y: 1625
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4313()
	}
	function onActivate4309() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16027
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 28 26 \"2\" ")
	}
	ButtonSq {
		id: button4309_1105
		x: 1680
		y: 993
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4309()
	}
	ButtonSq {
		id: button4314_1106
		x: 1680
		y: 1715
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4314()
	}
	function onActivate4311() 
	{
	 // Calling Command 28 (Simulate Keypress) with CommandGroup_Parameters from 16029
		manager.sendDceMessage(srouterip + " "+deviceid + " -203 1 28 26 \"4\" ")
	}
	ButtonSq {
		id: button4311_1107
		x: 1800
		y: 993
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/Wide%20KB.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4311()
	}
	ButtonSq {
		id: button4316_1108
		x: 1800
		y: 1715
		width: 120
		height: 90
		buttonbackground: "skins/Basic/Buttons/Keyboard/right.png"
		buttontext: ""
		clickHandler.onClicked: onActivate4316()
	}
}
