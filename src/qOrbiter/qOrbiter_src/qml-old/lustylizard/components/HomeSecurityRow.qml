import QtQuick 1.0

Rectangle {
    id: btHomeSecurity
    height: parent.height
    width: parent.height
    color: "transparent"
    radius: style.but_smooth

    Image {
        id: buttonbg
        source: "../img/ui3/securitybig.png"
        height: parent.height
        width: parent.width
    }

    MouseArea{
        id: mousearea1
        anchors.fill:parent

        onClicked:{

            manager.ShowFloorPlan(5)
            manager.setFloorplanType(5)
        }
    }
}


/*
Rectangle{
    id:
    clip:true
    color:"transparent"
    radius: 20

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
}

*/
