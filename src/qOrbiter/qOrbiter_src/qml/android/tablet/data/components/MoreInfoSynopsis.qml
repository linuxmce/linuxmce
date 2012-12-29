import QtQuick 1.1

Item {
    id: moreInfoSynopsis
    width: 1040
    height: 344
    clip: true;

    Rectangle {	
        id: background
        anchors.fill: parent
	color: "#000000"
	opacity: 0.8
    }

    Flickable {
        id: synopsisFlickable
	flickableDirection: Flickable.VerticalFlick
	interactive: true
	anchors.fill: parent
	contentHeight: synopsisText.paintedHeight + 40
        Text {
            id: synopsisText
	    font.family: "Roboto"
	    font.pixelSize: 24
	    font.bold: false
	    color: "#ffffff"
	    wrapMode: Text.WordWrap
	    anchors.fill: parent
	    anchors.leftMargin: 24
	    anchors.topMargin: 24
	    anchors.rightMargin: 24
	    text: "<b>Synopsis:</b><br />"+dcenowplaying.synop
        }

    }

    Image {
    	id: shadowOverSynopsis
        anchors.fill: parent
	source: "../img/synopsis_shadowbox.png"    
        opacity: 0.8
    }

    ScrollBar {
        flickable: synopsisFlickable
	vertical: true;
    }

}