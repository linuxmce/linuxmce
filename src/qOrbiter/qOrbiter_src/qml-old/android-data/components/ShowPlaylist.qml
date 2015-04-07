import QtQuick 1.0

Item {
    id: container
    signal clicked
    width: 270
    height: 270

    Rectangle {
        id: showPlaylistBackground
        color: "#33B5E5"
        anchors.fill: parent
        state: "RELEASED"

        states: [
            State {
                name: "PRESSED"
                PropertyChanges { target: showPlaylistBackground; opacity: 1.0 }
            },
            State {
                name: "RELEASED"
                PropertyChanges { target: showPlaylistBackground; opacity: 0.0 }
            }
        ]

        transitions: [
            Transition {
                from: "PRESSED"
                to: "RELEASED"
                PropertyAnimation { target: showPlaylistBackground; duration: 100 }
            },
            Transition {
                from: "RELEASED"
                to: "PRESSED"
                PropertyAnimation { target: showPlaylistBackground; duration: 100 }
            }
        ]
    }

    Image {
        id: showPlaylistIconImage
        source: "../img/PlaylistIcon.png"
	anchors.fill: parent
    }

    Connections {
        target: dcenowplaying
	onImageChanged: {
	   mininowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp;
	}
    }

    Image {
        id: mininowplayingimage
	anchors.fill:parent
	state: "NOTVISIBLE"
	fillMode: Image.PreserveAspectFit
	smooth: true
	clip: true
	states: [
	    State {
                name: "VISIBLE"
		PropertyChanges { target: mininowplayingimage; opacity: 1.0 }
	    },
            State {
	        name: "NOTVISIBLE"
		PropertyChanges { target: mininowplayingimage; opacity: 0.0 }
            }
	]
	transitions: [
            Transition {
	        from: "VISIBLE"
		to: "NOTVISIBLE"
		NumberAnimation { target: mininowplayingimage; properties: "opacity"; duration: 100 }
	    },
	    Transition {
	        from: "NOTVISIBLE"
		to: "VISIBLE"
		NumberAnimation { target: mininowplayingimage; properties: "opacity"; duration: 100 }
	    }
	]
    }

    MouseArea {
    	id: clickable
        anchors.fill: parent
        onPressed: showPlaylistBackground.state = "PRESSED"
        onReleased: showPlaylistBackground.state = "RELEASED"
	onClicked: { mininowplayingimage.opacity == 0.0 ? mininowplayingimage.state = "VISIBLE" : mininowplayingimage.state = "NOTVISIBLE"; container.clicked(); }
    }

}
