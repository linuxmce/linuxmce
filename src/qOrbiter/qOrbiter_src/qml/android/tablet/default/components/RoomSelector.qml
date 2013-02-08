import QtQuick 1.0
import com.nokia.android 1.1

import "../components"

//roomsList
// text: title + ":" + intRoom + ", In EA: " + entertain_area
// setActiveRoom(intRoom, entertain_area)
/*
                            setActiveRoom(intRoom, entertain_area)
                            rooms.state = "Default"
                            currentroom = title
                             roombutton.buttontext = title
  */

Rectangle {
    id:roomselectorrect
    height:manager.appHeight
    width: manager.appWidth
    color: "transparent"
    anchors.centerIn: parent
    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
    }

    Rectangle{
        anchors.fill: parent
        opacity: .85
        color:"black"
    }
    StyledText{
        anchors.bottom: roomsource.top
        fontSize: scaleY(7)
        text: qsTr("Please Select Location")
        color: "white"
    }

    ListView{
        id:roomsource
        anchors.centerIn: parent
        height: scaleY(45)
        width: manager.appWidth
        orientation: ListView.Horizontal
        spacing: 5
        clip: true
        model: roomList
        delegate: Item{
            height: scaleY(35)
            width: scaleX(20)
            ListItem{
                anchors.fill: parent
                Image {
                    id: roomimage
                    source: room_image
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                    anchors.fill: parent
                    opacity: .50
                }
                StyledText{
                    text: "Name:"+ name + "\n EA: " + ea_name
                    fontSize: scaleY(4)
                    isBold: true
                    anchors.centerIn: parent
                    color: "white"
                }
                onClicked: {
                    currentroom = name
                    setActiveRoom(intRoom, entertain_area)
                    //roombutton.buttontext = title
                    componentLoader.source=""
                }
            }
        }

        flickableDirection: "HorizontalFlick"
    }

}
