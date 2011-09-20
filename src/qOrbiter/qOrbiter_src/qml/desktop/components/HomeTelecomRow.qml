import QtQuick 1.0

Item {
    property alias rowlabel: rowheader.buttontext
    height: scaleY(17)
    width: scaleX(100)
    clip:true
    Rectangle{
        height: scaleY(17)
        width: scaleX(100)
        clip:true
        color:style.darkhighlight
        radius: 20
        Image {
            id: rowbgimg

            source: "../../../img/icons/rowbg.png"
            width: parent.width
            height: parent.height

        }
        HomeButtonDelegate{id:telecomDelegate}



        Row {
            id: telecomRow
            spacing:5
anchors.left: parent.left
            anchors.leftMargin: scaleX(2)

            ButtonSq {
                id: rowheader
                anchors.top: parent.top
                anchors.topMargin: scaleY(2)
                height: scaleY(13)
                width: scaleX(8)
                color: style.homescreenfloorplanbuttoncolor
                radius: style.but_smooth
                buttontext: "Telecom"
                Image {
                    id: buttonbg2
                    source: "../../../img/icons/buttonoverlay.png"
                    anchors.fill: rowheader
                    height: parent.height
                    width: parent.width
                }
                Image {
                    id: buttonbg
                    source: "../../../img/icons/Speed Dial.png"
                    anchors.fill: rowheader
                    height: parent.height
                    width: parent.width
                }
                MouseArea{
                    id: mousearea1
                    anchors.fill: parent
                    onClicked: showfloorplan(6)
                }

            }

            Flickable{
                id:securityflick
                height: scaleY(16)
                width: scaleX(87)
                anchors.left: rowheader.right
                anchors.leftMargin: 4
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
}
