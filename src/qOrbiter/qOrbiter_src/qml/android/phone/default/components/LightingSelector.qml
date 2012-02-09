// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id:genericlist
    height: 200

    HomeButtonDelegate{id:lightingdelegate}
    Image {
        id: bg
        source: "../img/bkg.png"
        anchors.fill: genericview
    }
    ListView{
        id: genericview
        width: 200
        height: genericlist.childrenRect.height +25
        model: currentRoomLights
        spacing:25
        orientation:ListView.Vertical
        delegate:  lightingdelegate
        interactive: true
        clip:true

    }
}
