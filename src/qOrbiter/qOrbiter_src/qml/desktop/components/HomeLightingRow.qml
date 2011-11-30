import QtQuick 1.0
Item {
    id:lightingrow
    height: scaleY(17)
    width: scaleX(parent.width)

    Rectangle{
        height: scaleY(17)
        width: scaleX(100)

        clip:true
color:"transparent"
        radius: 20

        HomeButtonDelegate{id:lightingdelegate}

        Row {
            id: guide
            spacing:2
            x: scaleX(2)


            ButtonSq {
                id: lightingfloorplan
                height: style.stdbuttonh
                width: style.stdbuttonw
                anchors.top: parent.top
                anchors.topMargin: scaleY(2)
                color: style.accentcolor
                radius: style.but_smooth
                buttontext: ""
                Image {
                    id: buttonbg
                    source: "../../../img/icons/buttonoverlay.png"
                    height: parent.height
                    width: parent.width
                }
                Image {
                    id: onimg
                    source: "../../../img/icons/jabber_protocol.png"
                    height: parent.height
                    width: parent.width
                }

                MouseArea{
                    id: mousearea1
                    onClicked: showfloorplan(2)
                }

            }
        Flickable{
            height: scaleY(15)
            width: scaleX(79)
            contentHeight: style.stdbuttonh
            contentWidth: ((style.stdbuttonw + 5) * (lightingScenarios.count + 1)) - 5
            clip: true
            flickableDirection: "HorizontalFlick"

                ListView{
                    id: lightingScenarios

                    width: stage.width
                    height: scaleY(style.stdbuttonh)
                    model: currentRoomLights
                    spacing: 5
                    orientation:ListView.Horizontal
                    delegate:  lightingdelegate
                    interactive: false

                }
            }
        }
    }
}

