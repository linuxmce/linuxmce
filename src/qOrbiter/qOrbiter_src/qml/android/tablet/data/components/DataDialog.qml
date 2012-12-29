/* A Dialog box for the Data Skin */

import QtQuick 1.1

Item {
     property real dialogWidth
     property real dialogHeight
     property string iconSource
     property string dialogTitle
     property Component contents
     property Component buttons
  
     id: container
     width: manager.appWidth
     height: manager.appWidth
     x: 0
     y: 0     
     z: 100

     /* The Lightbox backdrop */
     Rectangle {	
         id: lightbox
	 color: "#000000"
	 anchors.fill: parent
	 opacity: 0.7
     }

     Rectangle {
         id: lightsource
	 width: parent.dialogWidth
	 height: parent.dialogHeight
	 anchors.centerIn: parent
	 radius: 8
	 opacity: 1.0
	 gradient: Gradient {
	     GradientStop { position: 0.0; color: "#494949"; }
	     GradientStop { position: 0.1; color: "#282828"; }
	 }
	 
	 Rectangle {
	     id: background
	     color: "#282828"
	     width: parent.width - 4
	     height: parent.height - 4
	     x: 2
	     y: 2
	     radius: parent.radius
	     
	     Rectangle {
	         id: dialogBlueBar
		 x: 0
		 y: 96
		 width: parent.width
		 height: 2
		 color: "#33b5e5"	       
	     }
	     
	     Image {
	         id: dialogIcon
		 x: 16
		 y: 16
		 width: 64
		 height: 64
		 source: container.iconSource
	     }
	     
	     Text {
	         id: dialogTitle
	         x: 88
		 y: 32
		 font.pixelSize: 32
		 font.bold: false
		 font.family: "Roboto"
		 color: "#33b5e5"
		 text: container.dialogTitle
             }
	     
             Item {
                 id: contentFrame
                 width: parent.width - 48
		 height: parent.height - dialogBlueBar.y - 48
		 anchors.fill: parent
                 anchors.topMargin: 96 + 48
		 anchors.leftMargin: 48
		 anchors.rightMargin: 48
		 anchors.bottomMargin: 48
                 Loader {
		     anchors.centerIn: parent
		     width: contentFrame.width
		     sourceComponent: container.contents
		     anchors.verticalCenter: parent.verticalCenter
		 }
             }
	     
	     Item {
	         id: buttonsContainer
	         width: parent.width - 48
		 height: 81
		 x: 0
		 y: container.dialogHeight - 78
 		 Loader {
		     sourceComponent: buttons	
		 }	 
             }
	     
	 }

     }
          
}
