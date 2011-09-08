import QtQuick 1.0

Item {
    property alias rowlabel: rowheader.buttontext
    height: scaleY(17)
    width: scaleX(parent.width)

    Rectangle{
        height: scaleY(17)
        width: scaleX(100)
        clip:true
        color:"midnightblue"
        radius: 20
        Image {
            id: rowbgimg

            source: "../../../img/icons/rowbg.png"
            width: parent.width
            height: parent.height

        }
        HomeButtonDelegate{id:climateDelegate}

            Row {
                id: guide
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
                    buttontext: "Climate"

                    MouseArea{
                        id: mousearea1
                        anchors.fill: parent
                        onClicked:gotoQScreen("Screen_4.qml")
                        z:5 }
                }

                Flickable{

                    id:climateRow
                    height: scaleY(16)
                    width: scaleX(85)
                    anchors.centerIn: parent
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
