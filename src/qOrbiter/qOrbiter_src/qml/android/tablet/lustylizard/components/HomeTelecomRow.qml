import QtQuick 1.0

Rectangle {
    id: btHomeTelecom
    height: parent.height
    width: parent.height + scaleX(1)
    radius: style.but_smooth
    color:"#000000"
    opacity: 0.800
    Image {
        id: buttonbg
        source: "../images/btTelecom.png"
        height: Image.height
        width: Image.width
        anchors.top: parent.top
        anchors.topMargin: scaleX(1.5)
        anchors.horizontalCenter: btHomeTelecom.horizontalCenter
        opacity: 1
    }
    Text{
        id: text
        text: qsTr("Telecom")
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.horizontalCenter: btHomeTelecom.horizontalCenter
        font.pixelSize: 18
        color: "#ffffff"
    }
    MouseArea{
        id: mousearea1
        anchors.fill: rowheader
        onClicked: {
            manager.setFloorplanType(6)
            manager.ShowFloorPlan(6)

        }
    }
}

/*
Rectangle{
    id: btHomeTelecom
    clip:true
    color:"transparent"
    radius: 20

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
}

*/
