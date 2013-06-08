import QtQuick 2.0

Rectangle {
    height:scaleY(100)
    width:scaleX(100)

//	anchors.fill:item
    color: "blue"
    Text{
        text:"Tablet is loaded."
        anchors.centerIn: parent
        font.pointSize: 18
	color:"White"
    }
}
