import QtQuick 1.1
import "../js/NowPlaying_MediaAttributeHelpers.js" as NowPlaying_MediaAttributeHelpers

Item {
    id:moreInfo

    Connections {
        target: mediaplaylist
	onActiveItemChanged: {
 	    NowPlaying_MediaAttributeHelpers.clearEntries(moreInfoModel);
	    NowPlaying_MediaAttributeHelpers.appendEntries(moreInfoModel,dcenowplaying.qs_mainTitle,"Title");
	    NowPlaying_MediaAttributeHelpers.appendEntries(moreInfoModel,dcenowplaying.qs_mainTitle2,"Title 2");
	    NowPlaying_MediaAttributeHelpers.appendEntries(moreInfoModel,dcenowplaying.qs_subtitle,"Subtitle");
	    NowPlaying_MediaAttributeHelpers.appendEntries(moreInfoModel,dcenowplaying.episode,"Episode");
	    NowPlaying_MediaAttributeHelpers.appendEntries(moreInfoModel,dcenowplaying.director,"Director");
	    NowPlaying_MediaAttributeHelpers.appendEntries(moreInfoModel,dcenowplaying.genre,"Genre");
            NowPlaying_MediaAttributeHelpers.appendEntries(moreInfoModel,dcenowplaying.performerlist,"Performer");
	}
    }

    Rectangle {
        color: "#000000"
	opacity: 0.5
	anchors.fill: parent
    }

    ListModel {
        id: moreInfoModel
    }

    ListView {
        id: moreInfoView
        anchors.fill: parent
        clip: true
        interactive: true
        flickableDirection: "VerticalFlick"
	model: moreInfoModel
	delegate:
           Rectangle {
	       color: "#282828"
	       width: 880
	       height: 128
	       Image {
	           source: "../img/PlaylistItem.png"
		   width: 880
		   height: 128
		   anchors.centerIn: parent
		   clip: true
	       }
	       Text {
	           id: attributeName
		   width: 796
		   height: 64
		   font.family: "Roboto"
		   font.bold: true
		   color: "white"
		   font.pixelSize: 32
		   text: name
		   wrapMode: Text.NoWrap
		   elide: Text.ElideMiddle
		   x: 72
		   y: 0
		   verticalAlignment: Text.AlignBottom
		   horizontalAlignment: Text.AlignLeft
	       }
	       Text {
	           id: attributeDesc
		   width: 796
		   height: 64
		   font.family: "Roboto"
		   font.bold: false
		   color: "white"
		   font.pixelSize: 26
		   text: description
		   wrapMode: Text.NoWrap
		   elide: Text.ElideMiddle
		   x: 72
		   y: 64
		   verticalAlignment: Text.AlignTop
		   horizontalAlignment: Text.AlignLeft
		   textFormat: Text.StyledText 
	       }
	   }

    Text {
        id: "noinfo"
	font.bold: false
	anchors.centerIn: parent	 
	text: ( ( moreInfoModel.count == 0 ) ? "No Information Available" : "" )
	
	font.family: "Roboto"
	font.pixelSize: 72
	color: "#494949"
    }

    ScrollBar {
        flickable: moreInfoView 
        vertical: true
        }
    }
    
}
