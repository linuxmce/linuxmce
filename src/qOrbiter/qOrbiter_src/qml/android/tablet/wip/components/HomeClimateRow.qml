import QtQuick 1.0

Rectangle{
    id:btHomeClimates
    clip:false
    color:"#000000"
    opacity: 0.800
    visible: false
    width: scaleX(97)
    height: 100
    anchors.bottom: mainButtonContainer.top
    anchors.bottomMargin:  16
    anchors.horizontalCenter: stage.horizontalCenter

    property string arrowPos: "../images/btTelecom.png"

    HomeButtonDelegate{id:climateDelegate}

    Flickable{

        id:climateRow
        height: scaleY(16)
        width: scaleX(85)
        contentHeight: style.buttonH
        contentWidth: ((style.buttonW + 5) * (climateScenarios.count + 1)) - 5
        clip: true
        flickableDirection: "HorizontalFlick"

        ListView{
            id: climateScenarios
            height: scaleY(style.buttonH)
            width: stage.width
            model: currentRoomClimate
            spacing: 5
            orientation:ListView.Horizontal
            interactive:false
            delegate: climateDelegate
        }
    }
}

