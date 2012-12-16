import QtQuick 1.1
import com.nokia.android 1.1
import Qt.labs.shaders 1.0
import "components"

Window {  

	id: mainWindow
	// Component.onCompleted: manager.setBoundStatus(true)

	Image {
	        id: backg
		anchors.fill: parent
		source: "img/pictest.png"
		width: 1920
		height: 1200		
	}

	Rectangle {
		color: "black"
		width: 1920
		height: 84
		x: 0
		y: 0		
	}

	Rectangle {
		width: 64 
		height: 64
		x: 16
		y: 8
		radius: 16
		color: "#33b5e5"
	}

	Text {
		text: "Now Playing"
		font.pointSize: 18 
		font.family: "Roboto"
		font.bold: false 
		color: platformStyle.colorNormalLight
		x: 100
		y: 24 
	}

	Image {
		source: "img/media.png"
		width: 64
		height: 64
		x: 1700
		y: 8
		smooth: true	
	}

	Image {
		source: "img/lights.png"
		width: 64
		height: 64
		x: 1840 
		y: 8
		smooth: true
	}

	Rectangle
	{
		id: bluebar
		color: "#33b5e5"
		height: 2
		width: 1920
		x: 0
		y: 84
		z: 100 
	}

	Image {
	      id: topshadow
	      anchors.fill: backg
	      z: 99
	      source: "img/player_background.png"
	}

        NowPlayingBar {  
		id: nowPlaying
                y: 87
                x: 1040
		onClicked: {
		    nowplayingimage.state == "VISIBLE" ? nowplayingimage.state = "NOTVISIBLE" : nowplayingimage.state = "VISIBLE";
		    currentPlaylist.state == "NOTVISIBLE" ? currentPlaylist.state = "VISIBLE" : currentPlaylist.state = "NOTVISIBLE";
		}
        }

//    	Connections{
//        	target:dcenowplaying
//        	onImageChanged: {
//            		nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp;
//        	}
//    	}

	Playlist {
	    id: currentPlaylist
	    x: 0 
	    y: 86
	    z: 99
	    width: 1040
	    height: 1040
	    state: "NOTVISIBLE"
	    states: [
	        State {
		    name: "VISIBLE"
		    PropertyChanges { target: currentPlaylist; opacity: 1.0 }
		},
		State {
		    name: "NOTVISIBLE"
		    PropertyChanges { target: currentPlaylist; opacity: 0.0 }
		}	    
            ]
	    transitions: [
	        Transition {
		    from: "VISIBLE"
		    to: "NOTVISIBLE"
		    PropertyAnimation { target: currentPlaylist; property: "opacity"; duration: 100 }
		},
		Transition {
		    from: "NOTVISIBLE"
		    to: "VISIBLE"
		    PropertyAnimation { target: currentPlaylist; property: "opacity"; duration: 100 }
		}
	    ]
	}

        Rectangle {
            x: 0
	    y: 86
            width: 1040
	    height: 1040
            color: "#000000"
            opacity: 0.5
        }

	Menu {
	    x: 1140
	    y: 500
	    MenuItem {
	        text: "Text 1"	     
            }
	    MenuItem {
	        text: "Text 2"
	    }
	    MenuItem {
	    	text: "Text 3"
	    }
	    MenuItem {
	        text: "Text 4"
	    }
	}

	Image {
		id: nowplayingimage
		x: 0 
		y: 86 
		width: 1040 
		height: 1040
		clip: true
		fillMode: Image.PreserveAspectFit
		smooth: true
		state: "VISIBLE"
		states: [
		    State {
		        name: "VISIBLE"
			PropertyChanges { target: nowplayingimage; opacity: 1.0 }
		    },
		    State {
		        name: "NOTVISIBLE"
			PropertyChanges { target: nowplayingimage; opacity: 0.0 }
		    }
		]

		transitions: [
		    Transition {
		        from: "VISIBLE"
			to: "NOTVISIBLE"
			NumberAnimation { target: nowplayingimage; properties: "opacity"; duration: 100 }
		    },
		    Transition {
		        from: "NOTVISIBLE"
			to: "VISIBLE"
			NumberAnimation { target: nowplayingimage; properties: "opacity"; duration: 100 }
		    }
		]	
	}

	Image {
	      source: "img/cover_overlay.png"
	      anchors.fill: nowplayingimage
	      z: 99
	}

	MediaScrollBar {
		id:media_transit
		width: 684 
		x: 1140 
		y: 880
	}

	Row {
		width: 684
		height: 300
		x: 1140 
		y: 940 
		spacing: 180
		Button {
			width: 112 
			height: 112
			iconSource: "img/skip-back.png"
			onClicked: manager.newTrack("-1")
		}

		Button {
			width: 112
			height: 112
			iconSource: dcenowplaying.b_mediaPlaying ? "img/play.png" : "img/pause.png"
			onClicked: manager.pauseMedia()
		}

		Button {
			width: 112
			height: 112
			iconSource: "img/skip-fwd.png"
			onClicked: manager.newTrack("+1")
		}
	}

	OSDNavigation {
              y: 354
	      x: 1200
	}

}
