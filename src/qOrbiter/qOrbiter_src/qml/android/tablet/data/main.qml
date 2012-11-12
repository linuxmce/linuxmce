import QtQuick 1.1
import com.nokia.android 1.1
import Qt.labs.shaders 1.0
import "components"

Window {  

	id: mainWindow
	Component.onCompleted: manager.setBoundStatus(true)

  Image {
        id: transitPreview
        source: "img/harmattan_cross.png" 
        width: 1160 
        height: 654
	x: 732
	y: 112
	visible: true
        Connections{
            target: manager
            onMediaScreenShotReady: transitPreview.source="image://listprovider/screenshot/"+securityvideo.timestamp
        }
        Timer{
            id:previewTimer
            interval: 1000
            repeat: true
            running: true
            onTriggered: manager.getVideoFrame()
        }

    }

	Image {
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
		font.bold: true
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
	}

    	Connections{
        	target:dcenowplaying
        	onImageChanged: {
            		nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp;
            		console.log("now playing changed")
        	}
    	}

	Image {
		id: nowplayingimage
		x: 0 
		y: 86 
		width: 800 
		height: 1040
		clip: true
		fillMode: Image.PreserveAspectFit
		smooth: true	
	}

	Text {
		width: 832 
		height: 20
		x: 950 
		y: 720 
		text: dcenowplaying.qs_mainTitle
		font.family: "Roboto"
		font.bold: true
		font.pointSize: 16
		color: platformStyle.colorNormalLight
	}

	MediaScrollBar {
		id:media_transit
		width: 832 
		x: 950 
		y: 840
	}

	Row {
		width: 832
		height: 300
		x: 950 
		y: 900 
		spacing: 250 
		Button {
			width: 112 
			height: 112
			iconSource: "img/skip-back.png"
		}

		Button {
			width: 112
			height: 112
			iconSource: "img/pause.png"
		}

		Button {
			width: 112
			height: 112
			iconSource: "img/skip-fwd.png"
		}
	}

}
