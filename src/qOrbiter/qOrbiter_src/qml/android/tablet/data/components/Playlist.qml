import QtQuick 1.0

Item {
    id:playlist

    Rectangle {
        color: "#000000"
	opacity: 0.5
	anchors.fill: parent
    }

    Connections{
          target: dcenowplaying
          onPlayListPositionChanged:{
              playlistView.positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Beginning)
          }
      }

    ListView {
        id: playlistView
        anchors.fill: parent
        clip: true
        interactive: true
        flickableDirection: "VerticalFlick"
    model: manager.getDataGridModel("Playlist", 18)
	delegate:
           Rectangle {
	       color: index === dcenowplaying.m_iplaylistPosition ? "#FFFFFF" : "#282828"
	       width: 1040
	       height: 128
	       Image {
	           source: "../img/PlaylistItem.png"
		   width: 1040
		   height: 128
		   anchors.centerIn: parent
		   clip: true
	       }
	       Text {
	       	    id: nowPlayingText
		    font.family: "Roboto"
		    color: index === dcenowplaying.m_iplaylistPosition ? "black" : "white"
		    font.pixelSize: 32
		    text: name
		    wrapMode: "WrapAtWordBoundaryOrAnywhere"
		    font.bold: true 
		    x: 72
		    y: 24
	       }
	       MouseArea {
	           anchors.fill: parent
		   onClicked: manager.changedPlaylistPosition(index)
	       }
	   }
    ScrollBar {
        flickable: playlistView
        vertical: true
        }
    }
}
