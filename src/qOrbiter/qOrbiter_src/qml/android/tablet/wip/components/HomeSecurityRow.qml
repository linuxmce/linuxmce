import QtQuick 1.0

Rectangle{
    id: btHomeSecurity
    clip: true
    color:"#000000"
    opacity: 0.800
    visible: false

    width: scaleX(97)
    height: 100
    anchors.bottom: mainButtonContainer.top
    anchors.bottomMargin:  16
    anchors.horizontalCenter: parent.horizontalCenter

    HomeButtonDelegate{id:securityDelegate}

      Flickable  {
            id:securityflick
            height: scaleY(16)
            width: scaleX(95)
            flickableDirection: "HorizontalFlick"
            contentHeight: childrenRect.height
            contentWidth: ((style.buttonW + 5) * (securityScenarios.count + 1)) - 5
            clip: true

            ListView{
                id: securityScenarios
                height: scaleY(style.buttonH)
                width: stage.width
                model: currentRoomSecurity
                orientation:ListView.Horizontal
                spacing: 5

                delegate: securityDelegate
                interactive: false
            }
        }

}
