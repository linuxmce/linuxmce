import QtQuick 1.0
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
    height:100
    width: roomsource.width
    color: "transparent"
    anchors.centerIn: parent
    HomeButtonDelegate{id:lightingdelegate}
    Image {
        id: bg
        source: "../img/bkg.png"
        anchors.fill: genericview
    }

    ListView{
        id:genericview
        width: 200
        orientation: ListView.Vertical
        spacing: 5
        clip: true
        model: roomList
        delegate: lightingdelegate
        flickableDirection: "HorizontalFlick"


    }

}
