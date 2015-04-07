import QtQuick 2.0


    Rectangle{
        id:lightingrow
        height: scaleY(16)
        width: scaleX(100)

        clip:true
        color:"transparent"
        radius: 20

        HomeButtonDelegate{id:lightingdelegate}

        Row {
            id: guide
            spacing:5
            x: scaleX(2)

            Rectangle {
                id: lightingfloorplan
                height: skinStyle.stdbuttonh
                width: skinStyle.stdbuttonw
                anchors.top: parent.top
                anchors.topMargin: scaleY(2)
                color:"transparent"


                Image {
                    id: onimg
                    source: "../img/ui3/lightingbig.png"
                    height: parent.height
                    width: parent.width
                }

                MouseArea{
                    id: mousearea1
                    anchors.fill: parent
                    onClicked: {
                            manager.showfloorplan(2)
                       // manager.setFloorplanType(2)
                    }

                }

            }

            Flickable{
                height: scaleY(16)
                width: scaleX(79)
                contentHeight: skinStyle.stdbuttonh
                contentWidth: ((skinStyle.stdbuttonw + 5) * (lightingScenarios.count + 1)) - 5
                clip: false
                flickableDirection: "HorizontalFlick"

                ListView{
                    id: lightingScenarios
                    width: stage.width
                    height: scaleY(skinStyle.stdbuttonh)
                    model: currentRoomLights
                    spacing: 5
                    orientation:ListView.Horizontal
                    delegate:  lightingdelegate
                    interactive: false

                }
            }
        }
    }


