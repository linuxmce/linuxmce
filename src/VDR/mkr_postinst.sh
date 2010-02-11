#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

# Get the device ID of this system
. /usr/pluto/bin/Config_Ops.sh
ConfEval


# Get the video dir of VDR
VIDEO_DIR="/home/public/data/pvr"

# If the user already overwrote the default, get it.
. /etc/default/vdr

# We can't have VDR running during setup file edit
invoke-rc.d vdr stop
#
# Make a backup copy of the existing setup.conf
#
if [ -f /var/lib/vdr/setup.conf ]
	cp /var/lib/vdr/setup.conf `tempfile -d /var/lib/vdr -p setup -s \.conf`
fi
# The setup.conf defaults:
echo "AntiAlias = 1
AudioLanguages =
ChannelBlocker = 0
ChannelBlockerList =
ChannelBlockerMode = 0
ChannelEntryTimeout = 1000
ChannelInfoPos = 0
ChannelInfoTime = 5
CurrentChannel = 4
CurrentDolby = 0
CurrentVolume = 255
CutTimePatchEnabled = 0
DefaultLifetime = 99
DefaultPriority = 50
DelTimeshiftRec = 0
DisableVPS = 0
DiSEqC = 0
DisplaySubtitles = 0
DolbyTransferFix = 1
DoubleEpgAction = 0
DoubleEpgTimeDelta = 15
EmergencyExit = 1
EPGBugfixLevel = 3
EPGLanguages =
EPGLinger = 0
EPGScanTimeout = 5
FontFix = Courier:Bold
FontFixSize = 18
FontFixSizeP = 0.031000
FontOsd = Sans Serif:Bold
FontOsdSize = 22
FontOsdSizeP = 0.038000
FontSml = Sans Serif
FontSmlSize = 20
FontSmlSizeP = 0.035000
HardLinkCutter = 0
InitialChannel = 0
InitialVolume = -1
InstantRecordTime = 180
JumpPlay = 0
LnbFrequHi = 10600
LnbFrequLo = 9750
LnbSLOF = 11700
MarginStart = 2
MarginStop = 10
MarkInstantRecord = 1
MaxRecordingSize = 100
MaxVideoFileSize = 2000
MenuCmdPosition = 0
MenuKeyCloses = 0
MenuScrollPage = 1
MenuScrollWrap = 0
MinEventTimeout = 30
MinUserInactivity = 300
MixEpgAction = 0
MultiSpeedMode = 0
NameInstantRecord = TITLE EPISODE
NextWakeupTime = 1264359900
noEPGList =
noEPGMode = 0
NoQamOnDecoder = 0
OSDAspect = 1.422222
OSDHeight = 484
OSDHeightP = 0.840000
OSDLanguage =
OSDLeft = 58
OSDLeftP = 0.080000
OSDMessageTime = 1
OSDSkin = sttng
OSDTheme = default
OSDTop = 46
OSDTopP = 0.080000
OSDWidth = 624
OSDWidthP = 0.870000
PauseKeyHandling = 2
PauseLastMark = 0
PauseLifetime = 1
PausePriority = 10
PlayJump = 0
PrimaryDVB = 5
PrimaryLimit = 0
RecordDolbyDigital = 1
RecordingDirs = 1
ReloadMarks = 0
ResumeID = 0
SetSystemTime = 0
ShowInfoOnChSwitch = 1
ShowProgressBar = 0
ShowRecDate = 1
ShowRecLength = 0
ShowRecTime = 1
ShowReplayMode = 0
SourceCaps = 1 S28.2E
SourceCaps = 2 S19.2E
SourceCaps = 3 S19.2E
SourceCaps = 4 S19.2E
SplitEditedFiles = 0
SubtitleBgTransparency = 0
SubtitleFgTransparency = 0
SubtitleLanguages =
SubtitleOffset = 0
SVDRPTimeout = 300
TimeoutRequChInfo = 1
TimeSource = 0
TimeTransponder = 0
TurnOffPrimary = 0
UpdateChannels = 5
UseDolbyDigital = 1
UseSmallFont = 1
UseSubtitle = 1
UseSyncEarlyPatch = 0
UseVps = 0
VideoDisplayFormat = 1
VideoFormat = 0
VpsMargin = 120
ZapTimeout = 3
" > /var/lib/vdr/setup.conf

# First we add configuration settings for the plugins
#
# vdr-plugin-svdrpservice
#
echo "
svdrpservice.ConnectTimeout = 2
svdrpservice.ReadTimeout = 5
svdrpservice.ServerIp = 192.168.80.1
svdrpservice.ServerPort = 2001
" >> /var/lib/vdr/setup.conf
#
#
# vdr-plugin-streamdev-server
if [[ "$PK_Device" -eq "1" ]]; then
	echo "
streamdev-server.AllowSuspend = 1
streamdev-server.MaxClients = 6
streamdev-server.StartHTTPServer = 1
streamdev-server.StartServer = 1
streamdev-server.SuspendMode = 1
" >> /var/lib/vdr/setup.conf
else
	echo "
streamdev-client.RemoteIp = 192.168.80.1
streamdev-client.RemotePort = 2004
streamdev-client.StartClient = 1
streamdev-client.StreamFilters = 1
streamdev-client.SyncEPG = 0
" >> /var/lib/vdr/setup.conf
fi
# vdr-plugin-xineliboutput
echo "
xineliboutput.OSD.AlphaCorrection = 0
xineliboutput.OSD.AlphaCorrectionAbs = 0
xineliboutput.OSD.Blending = 0
xineliboutput.OSD.BlendingLowRes = 0
xineliboutput.OSD.DvbSubtitles = 0
xineliboutput.OSD.ExtSubSize = -1
xineliboutput.OSD.LayersVisible = 4
xineliboutput.OSD.Scaling = 2
xineliboutput.Post.denoise3d.Enable = 0
xineliboutput.Post.pp.Enable = 0
xineliboutput.Post.unsharp.Enable = 0
xineliboutput.Video.AspectRatio = 0
xineliboutput.Video.AutoCrop = 0
xineliboutput.Video.Decoder.H264 = automatic
xineliboutput.Video.Decoder.H264.SkipLoopFilter = all
xineliboutput.Video.Decoder.H264.SpeedOverAccuracy = yes
xineliboutput.Video.Decoder.MPEG2 = libmpeg2
xineliboutput.Video.Deinterlace = none
xineliboutput.Video.Driver = xv
xineliboutput.Video.FieldOrder = 0
xineliboutput.Video.Overscan = 0
xineliboutput.Video.Scale = 0
xineliboutput.Video.SwScale = 0" >> /var/lib/vdr/setup.conf

#
# for the MediaDirector
#
if [[ "$PK_Device" -ne "1" ]]; then
	echo "
remotetimers.AddToRemote = 1
remotetimers.DefaultUser = 0
remotetimers.HideMainMenuEntry = 0
remotetimers.RemoteInstant = 0
remotetimers.RemotePause = 0
remotetimers.ReplaceRecordings = 0
remotetimers.ReplaceSchedule = 1
remotetimers.ReplaceTimers = 1
remotetimers.ServerDir = $VIDEO_DIR
remotetimers.ServerIp = 192.168.80.1
remotetimers.ServerPort = 2001
remotetimers.ShowProgressBar = 0
remotetimers.SwapOkBlue = 0
remotetimers.UserFilterRecordings = 0
remotetimers.UserFilterSchedule = 0
remotetimers.UserFilterTimers = 0
remotetimers.WatchUpdate = 1
">> /var/lib/vdr/setup.conf
fi

# We restart VDR after the install on the core
if [[ "$PK_Device" -eq "1" ]]; then
        invoke-rc.d vdr restart
fi    
