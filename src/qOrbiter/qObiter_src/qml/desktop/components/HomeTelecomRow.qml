import QtQuick 1.0

Item {
    property alias rowlabel: rowheader.buttontext
    height: scaleY(16)
    width: scaleX(parent.width)


    HomeButtonDelegate{id:telecomDelegate}

    Flickable{
        id:securityflick

        height: parent.height
        width: parent.width
        contentHeight: style.buttonH
        contentWidth: ((style.buttonW) * (telecomScenarios.count + 1)) - 5
        clip: true
        flickableDirection: "HorizontalFlick"
        Row {
            id: telecomRow
            spacing:5

            ButtonSq {
                id: rowheader

                height: scaleY(16)
                width: scaleX(9)
                color: style.homescreenfloorplanbuttoncolor
                radius: style.but_smooth
                buttontext: "Telecom"
                MouseArea{
                    id: mousearea1
                    anchors.fill: parent
                    onClicked: gotoQScreen("Screen_6.qml")
                }

            }
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
