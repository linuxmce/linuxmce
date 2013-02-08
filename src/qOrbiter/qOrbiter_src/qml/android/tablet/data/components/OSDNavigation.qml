import QtQuick 1.1

Item {
	id: osdnavcontainer

	Timer {
	    id: upTimer
	    interval: 250; running: false; repeat: true;
	    onTriggered: manager.moveDirection(1);
	}

	Timer {
	    id: downTimer
	    interval: 250; running: false; repeat: true;
	    onTriggered: manager.moveDirection(2);
	}

	Timer {
	    id: leftTimer
	    interval: 250; running: false; repeat: true;
	    onTriggered: manager.moveDirection(3);
	}

	Timer {
	    id: rightTimer
	    interval: 250; running: false; repeat: true;
	    onTriggered: manager.moveDirection(4);
	}

	Timer {
	    id: enterTimer
	    interval: 250; running: false; repeat: true;
	    onTriggered: manager.moveDirection(5);
	}

	GraphicButton {
		      id: upButton
		      pressedImage: "../img/arrow_up_pressed.png"
		      unpressedImage: "../img/arrow_up_unpressed.png"
		      onClicked: manager.moveDirection(1) 
		      onPressAndHold: {
		          upTimer.start();
		      } 
		      onReleased: {
		          upTimer.stop();
		      } 
		      width: 272
		      height: 169
		      x: 156
		      y: 0		
	}

	OSDNavigationBack {
	    x: 0
	    y: 0
	}

	GraphicButton {
		      id: leftButton
		      pressedImage: "../img/arrow_left_pressed.png"
		      unpressedImage: "../img/arrow_left_unpressed.png"
		      onClicked: manager.moveDirection(3) 
		      onPressAndHold: {
		          leftTimer.start();
		      } 
		      onReleased: {
		          leftTimer.stop();
		      } 
		      width: 167
		      height: 270
		      x: 0
		      y: 158
	}

	GraphicButton {
		      id: enterButton
		      pressedImage: "../img/enter_pressed.png"
		      unpressedImage: "../img/enter_unpressed.png"
		      onClicked: manager.moveDirection(5) 
		      onPressAndHold: {
		          enterTimer.start();
		      } 
		      onReleased: {
		          enterTimer.stop();
		      } 
		      width: 256
		      height: 256
		      x: 160
		      y: 163	
	}

	GraphicButton {
		      id: rightButton
		      pressedImage: "../img/arrow_right_pressed.png"
		      unpressedImage: "../img/arrow_right_unpressed.png"
		      onClicked: manager.moveDirection(4) 
		      onPressAndHold: {
		          rightTimer.start();
		      } 
		      onReleased: {
		          rightTimer.stop();
		      } 
		      width: 171
		      height: 274
		      y: 156
		      x: 415	
	}

	GraphicButton {
		      id: downButton
		      pressedImage: "../img/arrow_down_pressed.png"
		      unpressedImage: "../img/arrow_down_unpressed.png"
		      onClicked: manager.moveDirection(2) 
		      onPressAndHold: {
		          downTimer.start();
		      } 
		      onReleased: {
		          downTimer.stop();
		      } 
		      width: 272
		      height: 168
		      x: 156
		      y: 418	
	}
	


}
