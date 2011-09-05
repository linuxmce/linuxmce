import QtQuick 1.0
Item {

    height: scaleY(16)
    width: scaleX(parent.width)

    HomeButtonDelegate{id:lightingdelegate}
    Flickable{
        height: parent.height
        width: parent.width
        contentHeight: style.buttonH
        contentWidth: ((style.buttonW + 5) * (lightingScenarios.count + 1)) - 5
        clip: true
        flickableDirection: "HorizontalFlick"
        Row {
            id: guide
            //anchors.left: lightingfloorplan.right
            spacing:5

            ButtonSq {
                id: lightingfloorplan
                height: scaleY(16)
                width: scaleX(9)

                color: style.homescreenfloorplanbuttoncolor
                radius: style.but_smooth
                buttontext: ""

                Image {
                    id: onimg
                    source: "../../../img/icons/jabber_protocol.png"
                    height: scaleY(16)
                    width: scaleX(9)
                    anchors.centerIn: parent

                }
                MouseArea{
                    id: mousearea1
                    anchors.fill: parent
                    onClicked: showfloorplan(2)

                }

            }

            ListView{
                id: lightingScenarios
                width: stage.width
                height: scaleY(style.buttonH)
                model: currentRoomLights
                spacing: 5
                orientation:ListView.Horizontal
                delegate:  lightingdelegate
                interactive: false
            }
        }
    }
}

