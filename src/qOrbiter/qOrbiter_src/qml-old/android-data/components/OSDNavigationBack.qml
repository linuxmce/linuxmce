import QtQuick 1.1

Item {
    id: container

    signal clicked
    signal pressAndHold

    width: 128
    height: 128

    Rectangle {
        anchors.fill: parent
	color: "#1c1d1e"
	border.color: "#33B5E5"
	border.width: 1
    }

    Rectangle {
        id: buttonBackground
	color: "#33B5E5"
	anchors.fill: parent
	state: "RELEASED"
        states: [
            State {
                name: "PRESSED"
                PropertyChanges { target: buttonBackground; opacity: 1.0 }
            },
            State {
                name: "RELEASED"
                PropertyChanges { target: buttonBackground; opacity: 0.0 }
            }
        ]

        transitions: [
            Transition {
                from: "PRESSED"
                to: "RELEASED"
                PropertyAnimation { properties: "opacity"; target: buttonBackground; duration: 100 }
            },
            Transition {
                from: "RELEASED"
                to: "PRESSED"
                PropertyAnimation { properties: "opacity"; target: buttonBackground; duration: 100 }
            }
        ]
	
    }

    Image {
        id: buttonImage
	source: "../img/1_navigation_back.png"
	width: 64
	height: 64
	anchors.centerIn: parent
    }

    MouseArea {
    	id: clickable
        anchors.fill: parent
        onPressed: buttonBackground.state = "PRESSED"
        onReleased: buttonBackground.state = "RELEASED"
	onClicked: { container.clicked(); }
	onPressAndHold: { container.pressAndHold(); } 
    }


}