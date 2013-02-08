import QtQuick 1.1

Item {
    id: container
    signal clicked

    width: 880
    height: 270

    Rectangle {
        anchors.fill: parent
	color: "#000000"
	opacity: 0.5
    }
    Text {
        x: 51
        y: 0
	width: 829
	height: 135
        verticalAlignment: Text.AlignBottom
	font.bold: true
	font.pixelSize: 24
	color: "#ffffff"
	text: dcenowplaying.qs_mainTitle
    }
    Text {
    	 x: 51
	 y: 135
	 width: 829
	 height: 135
	 verticalAlignment: Text.AlignTop
	 font.bold: false
	 font.pixelSize: 24
	 color: "#ffffff"
	 text: dcenowplaying.qs_subTitle
    }
    ShowPlaylist {
         id: showButton
         x: 610
	 y: 0
	 onClicked: container.clicked()
    }
}