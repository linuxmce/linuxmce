import QtQuick 1.1

Rectangle{
    height: scaleY(16)
    width: scaleX(100)
    clip:true
    color:"transparent"
    radius: 20    

    Row {
        id: telecomRow
        spacing:5
        x: scaleX(2)

        Rectangle {
            id: rowheader
            anchors.top: parent.top
            anchors.topMargin: scaleY(2)
            height: scaleY(13)
            width: scaleX(8)
            radius: skinstyle.but_smooth
            color:"transparent"

            Image {
                id: buttonbg
                source: "../img/ui3/telephonebig.png"
                height: parent.height
                width: parent.width
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

        Flickable{
            id:securityflick
            height: scaleY(16)
            width: scaleX(87)
            contentHeight: skinstyle.buttonH
            contentWidth: ((skinstyle.buttonW) * (telecomScenarios.count + 1)) - 5
            clip: true
            flickableDirection: "HorizontalFlick"

            ListView{
                id: telecomScenarios
                height: scaleY(skinstyle.buttonH)
                width: stage.width
                model: currentRoomTelecom
                spacing: 5
                interactive: false
                orientation:ListView.Horizontal
                delegate: HomeButtonDelegate{}
            }
        }
    }
}

