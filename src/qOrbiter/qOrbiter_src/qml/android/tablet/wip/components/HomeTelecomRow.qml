import QtQuick 1.0

Rectangle{
    id: btHomeTelecom
    clip:true
    color:"#000000"
    opacity: 0.800
    visible: false

    width: scaleX(97)
    height: 100
    anchors.bottom: mainButtonContainer.top
    anchors.bottomMargin:  16
    anchors.horizontalCenter: parent.horizontalCenter

    HomeButtonDelegate{id:telecomDelegate}

        Flickable{
            id:securityflick
            height: scaleY(16)
            width: scaleX(87)
            contentHeight: style.buttonH
            contentWidth: ((style.buttonW) * (telecomScenarios.count + 1)) - 5
            clip: true
            flickableDirection: "HorizontalFlick"

            ListView{
                id: telecomScenarios
                height: scaleY(style.buttonH)
                width: stage.width
                model: currentRoomTelecom
                spacing: 5
                interactive: false
                orientation:ListView.Horizontal
                delegate: telecomDelegate
            }
        }

}

