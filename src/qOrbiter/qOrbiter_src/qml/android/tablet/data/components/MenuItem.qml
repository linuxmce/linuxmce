import QtQuick 1.1

Item {

    property bool showSeperator: true 
    property string text

    signal clicked

    id: container
    width: parent.width
    height: 72
    
    Rectangle {
        id: mainArea
	color: "#303030"
	state: "RELEASED"
	width: container.width
	height: container.height
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
        x: 18
	y: 18
	text: parent.text
	color: "#FFFFFF"
	font.family: "Roboto"
	font.pixelSize: 26
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