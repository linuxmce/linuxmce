// import QtQuick 2.2 // to target S60 5th Edition or Maemo 5
import QtQuick 2.2

Rectangle {
    id:lightingmenu
    height: 100

    HomeButtonDelegate{id:lightingdelegate}
    Image {
        id: bg
        source: "../img/bkg.png"
        anchors.fill: lightingScenarios
    }
    ListView{
        id: lightingScenarios
        width: 200
        height: lightingScenarios.childrenRect.height +25
        model: currentRoomLights
        spacing:25
        orientation:ListView.Vertical
        delegate:  lightingdelegate
        interactive: true
        clip:true

    }
}
