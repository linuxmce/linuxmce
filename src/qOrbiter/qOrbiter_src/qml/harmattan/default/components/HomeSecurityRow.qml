import QtQuick 1.0

Item {

    height: style.homescreenrowheight
    width: parent.width


HomeButtonDelegate{id:securityDelegate}

Flickable
{
    id:securityflick
    flickableDirection: "HorizontalFlick"
    height: style.rowH
    width: parent.width
    contentHeight: childrenRect.height
    contentWidth: childrenRect.width * 2
    clip: true

        ButtonSq {
            id: rowheader

            height: style.buttonH
            width: style.buttonW
            color: style.homescreenfloorplanbuttoncolor
            radius: style.but_smooth
            buttontext: "Security Floorplan"
            MouseArea{
                id: mousearea1
                anchors.fill: parent
                onClicked:setCurrentScreen("Screen_5.qml")
              }
        }

        ListView{
            id: securityScenarios
            width: stage.width
            height: 50
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

