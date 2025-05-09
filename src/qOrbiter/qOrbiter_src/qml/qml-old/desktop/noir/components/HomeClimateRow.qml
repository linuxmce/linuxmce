import QtQuick 1.1



    Rectangle{
        id:climaterow
        height: scaleY(16)
        width: scaleX(100)
        clip:true
color:"transparent"
        radius: 20

        HomeButtonDelegate{id:climateDelegate}

        Row {
            id: guide
            spacing:5
            x: scaleX(2)
            Rectangle {
                id: rowheader
                anchors.top: parent.top
                anchors.topMargin: scaleY(2)
                height: scaleY(13)
                width: scaleX(8)
                radius: style.but_smooth
                color:"transparent"

                MouseArea{
                    id: mousearea1
                    anchors.fill: parent
                    onClicked:{
                        manager.setFloorplanType(4)
                        manager.ShowFloorPlan(4)

                     }
                }


                Image {
                    id: onimg
                    source: "../img/ui3/climatebig.png"
                    width: parent.width
                    height: parent.height

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

