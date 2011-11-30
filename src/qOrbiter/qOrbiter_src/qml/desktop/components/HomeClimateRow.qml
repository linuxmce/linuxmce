import QtQuick 1.0

Item {
    property alias rowlabel: rowheader.buttontext
    height: scaleY(17)
    width: scaleX(parent.width)

    Rectangle{
        height: scaleY(17)
        width: scaleX(100)
        clip:true
color:"transparent"
        radius: 20

        HomeButtonDelegate{id:climateDelegate}

        Row {
            id: guide
            spacing:5
            x: scaleX(2)
            ButtonSq {
                id: rowheader
                anchors.top: parent.top
                anchors.topMargin: scaleY(2)
                height: scaleY(13)
                width: scaleX(8)
                radius: style.but_smooth
                buttontext: qsTr("Climate")

                MouseArea{
                    id: mousearea1
                    onClicked:showfloorplan(4)
                    z:5 }

                Image {
                    id: buttonbg
                    source: "../../../img/icons/buttonoverlay.png"
                    height: parent.height
                    width: parent.width
                }

                Image {
                    id: onimg
                    source: "../../../img/icons/kweather.png"
                    height: parent.height
                    width: parent.width

                }
            }

            Flickable{

                id:climateRow
                height: scaleY(16)
                width: scaleX(85)
                contentHeight: style.buttonH
                contentWidth: ((style.buttonW + 5) * (climateScenarios.count + 1)) - 5
                clip: true
                flickableDirection: "HorizontalFlick"

                ListView{
                    id: climateScenarios
                    height: scaleY(style.buttonH)
                    width: stage.width
                    model: currentRoomClimate
                    spacing: 5
                    orientation:ListView.Horizontal
                    interactive:false
                    delegate: climateDelegate
                }

            }
        }
    }
}
