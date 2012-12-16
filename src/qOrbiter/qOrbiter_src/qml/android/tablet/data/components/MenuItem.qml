import QtQuick 1.1

Item {

    property bool showSeperator: true 
    property string text

    signal clicked

    id: container
    width: menuText.length * 12 + 196
    height: 72
    
    Rectangle {
        id: seperatorLine
        anchors.fill: parent
	color: "#505050"
	visible: parent.showSeperator
	y: parent.y-1    
    }

    Rectangle {
        id: mainArea
        anchors.fill: parent
	color: "#303030"
	state: "RELEASED"
	states: [
	    State {
	        name: "PRESSED"
		PropertyChanges { target: mainArea; color: "#33b5e5" }
	    },
	    State {
	        name: "RELEASED"
		PropertyChanges { target: mainArea; color: "#303030" }
	    }
	]
	transitions: [
	    Transition {
	        from: "PRESSED"
		to: "RELEASED"
		ColorAnimation { target: mainArea; duration: 100 }
	    },
	    Transition {
	        from: "PRESSED"
		to: "RELEASED"
		ColorAnimation { target: mainArea; duration: 100 }
	    }
	]
    }

    Text {
        id: menuText
        x: 12
	y: 24
	text: parent.text
	color: "#FFFFFF"
	font.family: "Roboto"
	font.pixelSize: 24
	font.bold: false	 
    }

    MouseArea {
        id: clickable
	anchors.fill: parent
	onPressed: {
	    mainArea.state = "PRESSED";
	}
	onReleased: {
	    mainArea.state = "RELEASED";
	}
	onClicked: {
	    container.clicked();
	}
    }    
}