import QtQuick 1.1

Item {
	id: container

	signal clicked
	signal pressAndHold
	signal released

        property string pressedImage
	property string unpressedImage

	Image {
  		id: pressedImage
		source: parent.pressedImage
		anchors.fill: parent
	}

	Image {
  		id: unpressedImage
		source: parent.unpressedImage
		anchors.fill: parent
		state: "RELEASED"
		states: [
			State {
			      name: "PRESSED"
			      PropertyChanges { target: unpressedImage; opacity: 0.0 }
			},
			State {
			      name: "RELEASED"
			      PropertyChanges { target: unpressedImage; opacity: 1.0 }
			}
		]
		
		transitions: [
			     Transition {
			     	from: "PRESSED"
				to: "RELEASED"
				PropertyAnimation { properties: "opacity"; target: unpressedImage; duration: 50 }
			     },
			     Transition {
			        from: "RELEASED"
				to: "PRESSED"
				PropertyAnimation { properties: "opacity"; target: unpressedImage; duration: 50 }
			     }
		]
	}

	MouseArea {
		  id: clickable
		  anchors.fill: parent
		  onPressed: {
		  	     unpressedImage.state = "PRESSED";
		  }
		  onReleased: {
		  	     unpressedImage.state = "RELEASED";
			     container.released();
		  }
		  onClicked: {
			     container.clicked();
		  }
		  onPressAndHold:
		  {
		             container.pressAndHold();
		  }
	}	
}
