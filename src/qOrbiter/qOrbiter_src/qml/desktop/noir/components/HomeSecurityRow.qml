import QtQuick 1.1



Rectangle{
    id:securityrow
    height: scaleY(16)
    width: scaleX(100)
    clip:true
    color:"transparent"
    radius: 20

    HomeButtonDelegate{id:securityDelegate}

    Row {
        id: guide
        spacing:5
        x: scaleX(2)
        Rectangle {
            id: rowheader
            height: scaleY(13)
            width: scaleX(8)
            anchors.top: parent.top
            anchors.topMargin: scaleY(2)
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

        Flickable
        {
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

