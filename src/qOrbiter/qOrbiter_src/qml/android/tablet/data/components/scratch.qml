Rectangle {

	color: "#000000"
    width: manager.appWidth
    height: manager.appWidth
	x: 0
	y: 0
	z: 100
	opacity: 0.7

	/* This is the gradient border around the rectangle, acting as a faux light source */
	Rectangle {
		id: borderRect
		width: 1254
		height: 516
		x: 314
		y: 336
		radius: 8
		z: 101;
		opacity: 1.0
		gradient: Gradient {
			GradientStop { position: 0.0; color: "#494949"; }
			GradientStop { position: 0.1; color: "#282828"; }	
		}

			/* The Content rectangle nests inside */	
			Rectangle {
			id: mainRect
			color: "#282828"
			width: parent.width - 4
			height: parent.height - 4
			x: 2
			y: 2
			z: 102
			radius: 8

			/* The Blue title bar */
			Rectangle {
				  id: dialogBlueBar
				  x: 0
				  y: 96
				  width: parent.width
				  height: 2
				  color: "#59adc9"

			}

			/* The Dialog icon */
			Image {
			      id: dialogIcon
			      x: 16
			      y: 16
			      width: 64
			      height: 64
			      source: "../img/lights.png"
			 }

			 /* The Dialog Title */
			 Text {	
			      x: 88
			      y: 32 
			      id: dialogTitle
			      font.pixelSize: 32
			      font.bold: false
			      font.family: "Roboto Regular"
			      color: "#59adc9"
			      text: "Adjust Lighting"
			 }

			 VolumeScroller {
			 	x: 32
				y: 240
				width: 1190
			 }

			 /* The next two rectangles provide the dialog box button frames */
			 Rectangle {
			     width: parent.width
			     height: 2
			     x: 0
			     y: parent.height - 81
			     color: "#494949"
			 }

			Rectangle {
			    width: 2
		 	    height: 81	
			    x: parent.width / 2
			    y: mainRect.height - 81
			    color: "#494949"
		        }

			Row {
			    width: 1254
			    height: 81
			    spacing: 4
			    x: 0
			    y: mainRect.height - 78
			    DataDialogButton {
			        buttonText: "Cancel"
			    }
			    DataDialogButton {
			        buttonText: "OK"
			    }
 
			}

	          }
	
	          Image {
		       /* source: "../img/DataDialog_Shadow.png" */
		       width: borderRect.width
		       height: borderRect.height
		       x: borderRect.x + 4
		       y: borderRect.y + 4
		       z: 99
		  }
        }
}
