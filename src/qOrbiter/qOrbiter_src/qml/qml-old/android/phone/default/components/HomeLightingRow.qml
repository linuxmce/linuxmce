import QtQuick 1.0


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
            spacing:scaleX(1)
            x: scaleX(2)

            Rectangle {
                id: lightingfloorplan
                height: style.stdbuttonh
                width: style.stdbuttonw
                anchors.top: parent.top
                anchors.topMargin: scaleY(2)
                color:"transparent"


                Image {
                    id: onimg
                    source: "../images/lightingbig.png"
                    height: parent.height
                    width: parent.width
                }

                MouseArea{
                    id: mousearea1
                    onClicked: showfloorplan(2)
                }

            }

            Flickable{
                height: scaleY(16)
                width: scaleX(79)
                contentHeight: style.stdbuttonh
                contentWidth: ((style.stdbuttonw + 5) * (lightingScenarios.count + 1)) - 5
                clip: false
                flickableDirection: "HorizontalFlick"

                ListView{
                    id: lightingScenarios
                    width: stage.width
                    height: scaleY(style.stdbuttonh)
                    model: currentRoomLights
                    spacing: scaleX(5)
                    orientation:ListView.Horizontal
                    delegate:  lightingdelegate
                    interactive: false

                }
            }
        }
    }


