import QtQuick 1.1
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

Item {
    id:roomSelector
    anchors.fill: parent

    Rectangle{
        id:phil
        color: "black"
        anchors.centerIn: parent
        anchors.fill: parent
        opacity:.65
        MouseArea{
            id:block
            anchors.fill: parent
        }
    }

    Rectangle{
        id:roomselectorheader
        height: scaleY(10)
        anchors.centerIn: parent
        anchors.verticalCenterOffset: scaleY(-20)
        width:parent.width
        color:"slategrey"

        StyledText {
            id: headertext
            text: "Select Room Please"
            anchors.centerIn: parent
        }

    }
    ListView{
        id:roomsource
        anchors.top: roomselectorheader.bottom
        anchors.horizontalCenter: roomselectorheader.horizontalCenter
        height: scaleY(45)
        width: parent.width
        orientation: ListView.Horizontal
        spacing: 5
        clip: true
        model: roomList
        delegate:
            RoomSelectionDelegate {
                id: roomselectioncomponent
            }
        flickableDirection: "HorizontalFlick"


    }

}
