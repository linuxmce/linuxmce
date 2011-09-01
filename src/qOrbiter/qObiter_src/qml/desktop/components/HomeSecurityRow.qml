import QtQuick 1.0

Item {

    height: scaleY(16)
    width: scaleX(parent.width)


HomeButtonDelegate{id:securityDelegate}

Flickable
{
    id:securityflick
    flickableDirection: "HorizontalFlick"
    height:parent.height
    width: parent.width
    contentHeight: childrenRect.height
    contentWidth: ((style.buttonW + 5) * (securityScenarios.count + 1)) - 5
    clip: true

        ButtonSq {
            id: rowheader

            height: scaleY(16)
            width: scaleX(9)
            color: style.homescreenfloorplanbuttoncolor
            radius: style.but_smooth
            buttontext: "Security Floorplan"
            MouseArea{
                id: mousearea1
                anchors.fill: parent
                onClicked:gotoQScreen("Screen_5.qml")
              }
        }

        ListView{
            id: securityScenarios
            height: scaleY(style.buttonH)
            width: stage.width
            model: currentRoomSecurity
            orientation:ListView.Horizontal
            spacing: 5
            anchors.left: rowheader.right
            anchors.margins: 5
            delegate: securityDelegate
            interactive: false
                 }
           }
       }

