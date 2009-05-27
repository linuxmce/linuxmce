#!/bin/bash
function svdrp_lstc()
{
	local lstc
lstc="220 linux2008 SVDRP VideoDiskRecorder 1.6.0-1; Thu Feb 19 09:28:50 2009
250-1 Das Erste;ARD:11837:hC34:S19.2E:27500:101:102=deu,103=2ch;106=dd:104:0:28106:1:1101:0
250-2 ZDF;ZDFvision:11954:hC34:S19.2E:27500:110:120=deu,121=2ch;125=deu:130:0:28006:1:1079:0
250-1827 TVP1;TVP:10773:hC56:S19.2E:22000:517:700=pol:705:9B2,9D0:17105:1:1053:0
250-1828 TVP2;TVP:10773:hC56:S19.2E:22000:518:710=pol:715:9B2,9D0:17106:1:1053:0
250-1829 TVP INFO;TVP:10773:hC56:S19.2E:22000:519:720=pol:725:0:17107:1:1053:0
250 1830 TVP Sport;TVP:10773:hC56:S19.2E:22000:520:730=pol:0:9B2,9D0:17108:1:1053:0
221 linux2008 closing connection"

	echo "$lstc"
}

VDRIP="192.168.80.1"

while read line; do
	Code="${line:0:3}"
	if [[ "$Code" != 250 ]]; then
		continue
	fi
	Ch4="${line:3:1}"
	AnswerLine="${line:4}"
	ChannelData="${AnswerLine%%;*}"
	ChannelName="${ChannelData#* }"
	ChannelNumber="${ChannelData%% *}"

	Filename="$ChannelName.vdrs"
	URL="http://$VDRIP:3000/TS/$ChannelNumber"
	echo "$Filename: URL=$URL"
done < <(svdrp_lstc)
