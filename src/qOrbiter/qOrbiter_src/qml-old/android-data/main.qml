import QtQuick 1.1
import com.nokia.android 1.1
import Qt.labs.shaders 1.0
import "components"

orbiterWindow {  

	id: mainorbiterWindow
	Component.onCompleted: manager.setBoundStatus(true)

	Image {
	        id: backg
		anchors.fill: parent
		source: "img/pictest.png"
        width: manager.appWidth
        height: manager.appWidth
	}

	Rectangle {
		color: "black"
        width: manager.appWidth
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

        Tabs {
	    id: actionTabs
	    itemWidth: 200
	    x: 96
	    y: 0
	    actions: ListModel {
 	        ListElement {
                    itemText: "NOW PLAYING"
		    itemIndex: 0
                }
	        ListElement {
	            itemText: "MORE INFO"
		    itemIndex: 1
	        }
	    }
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
        width: manager.appWidth
		x: 0
		y: 84
		z: 100 
	}

    	Connections{
        	target:dcenowplaying
        	onImageChanged: {
            		nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp;
        	}
    	}

	MoreInfo {
	    id: moreInfo
            x: 1040
	    y: 86
	    z: 99
	    width: 880
	    height: 1040
	    state: ((actionTabs.selectedIndex == 1) ? "VISIBLE" : "NOTVISIBLE")
	    states: [
	        State {
		    name: "VISIBLE"
		    PropertyChanges { target: moreInfo; opacity: 1.0 }
		    PropertyChanges { target: currentPlaylist; opacity: 0.0 }
		    PropertyChanges { target: nowplayingimage; opacity: 1.0 } 
		},
		State {
		    name: "NOTVISIBLE"
		    PropertyChanges { target: moreInfo; opacity: 0.0 }
		}
	    ]
	    transitions: [
	        Transition {
		    from: "VISIBLE"
		    to: "NOTVISIBLE"
		    PropertyAnimation { target: moreInfo; property: "opacity"; duration: 100 }
		},
		Transition {
		    from: "NOTIVISIBLE"
		    to: "VISIBLE"
		    PropertyAnimation { target: moreInfo; property: "opacity"; duration: 100 }
		    PropertyAnimation { target: moreInfo; property: "opacity"; duration: 100 }
		    PropertyAnimation { target: moreInfo; property: "opacity"; duration: 100 }
		}
	    ]
	}

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

	Image {
		id: nowplayingimage
		x: 0 
		y: 86 
		width: 1040 
		height: 1040
		clip: true
		fillMode: Image.PreserveAspectFit
		smooth: false
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

	MoreInfoSynopsis {
	    id: moreInfoSynop
            anchors.bottom: nowplayingimage.bottom
	    state: ( (actionTabs.selectedIndex == 1) ? "VISIBLE" : "INVISIBLE" )
	    visible: ( ((actionTabs.selectedIndex == 1) && dcenowplaying.synop.length > 0) ? true : false )
	    states: [
	        State {
		    name: "VISIBLE"
		    PropertyChanges { target: moreInfoSynop; opacity: 1.0 }
		},
		State {
		    name: "NOTVISIBLE"
		    PropertyChanges { target: moreInfoSynop; opacity: 0.0 }
		}
	    ]
	    transitions: [
	        Transition {
		    from: "VISIBLE"
		    to: "NOTVISIBLE"
		    NumberAnimation { target: moreInfoSynop; properties: "opacity"; duration: 100 }
		},
		Transition {
		    from: "NOTVISIBLE"
		    to: "VISIBLE"
		    NumberAnimation { target: moreInfoSynop; properties: "opacity"; duration: 100 }
		}
	    ]
	}

	// Image {
	//      source: "img/cover_overlay.png"
	//      anchors.fill: nowplayingimage
	//      z: 20
	// }

        Item {
	    id: nowPlayingBits;
	    state: ( (actionTabs.selectedIndex == 0) ? "VISIBLE" : "INVISIBLE" )
	    states: [
	        State {
		    name: "VISIBLE"
		    PropertyChanges { target: nowPlayingBits; opacity: 1.0 }
		},
		State {
		    name: "INVISIBLE"
		    PropertyChanges { target: nowPlayingBits; opacity: 0.0 }
		}
	    ]
	    transitions: [
	        Transition {
		    from: "VISIBLE"
		    to: "INVISIBLE"
		    PropertyAnimation { target: nowPlayingBits; properties: "opacity"; duration: 100 }
		},
		Transition {
		    from: "INVISIBLE"
		    to: "VISIBLE"
		    PropertyAnimation { target: nowPlayingBits; properties: "opacity"; duration: 100 }
		}
	    ]
	    

            NowPlayingBar {  
    	        id: nowPlaying
                y: 87
                x: 1040
		onClicked: {
		    nowplayingimage.state == "VISIBLE" ? nowplayingimage.state = "NOTVISIBLE" : nowplayingimage.state = "VISIBLE";
		    currentPlaylist.state == "NOTVISIBLE" ? currentPlaylist.state = "VISIBLE" : currentPlaylist.state = "NOTVISIBLE";
		}
            }

	    MediaScrollBar {
	    	id:media_transit
		width: 684 
		x: 1140 
		y: 880
	    }

	    Text {
	        id: currentTime
		text: dceTimecode.qsCurrentTime
		font.bold: false
		font.family: "Roboto"
		font.pixelSize: 20
		color: "#ffffff"
		x: 1045
		y: 870
	    }

	    Text {
	        id: durationTime
		text: dceTimecode.qsTotalTime
		font.bold: false
		font.pixelSize: 20
		x: 1837
		y: 870
		color: "#ffffff"
	    }

	    DataDialog { 
	        dialogWidth: 1140
		dialogHeight: 500
		iconSource: "../img/lights.png"
		dialogTitle: "Adjust Lighting"
                contents: Scroller { }
		buttons: DataDialogButtonRow {
		    DataDialogButton {
		        buttonText: "OK"
		    }
		    DataDialogButton {
		        buttonText: "CANCEL"
		    }
		}
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
	}

}
