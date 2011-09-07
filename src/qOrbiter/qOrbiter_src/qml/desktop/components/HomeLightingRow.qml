import QtQuick 1.0
Item {
    id:lightingrow
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
        // anchors.centerIn: lightingrow
        HomeButtonDelegate{id:lightingdelegate}

        Row {
            id: guide
            //anchors.left: lightingfloorplan.right
            spacing:2
            anchors.left: parent.left
            anchors.leftMargin: scaleX(2)

            ButtonSq {
                id: lightingfloorplan
                height: scaleY(13)
                width: scaleX(8)
                anchors.top: parent.top
                anchors.topMargin: scaleY(2)
                color: style.homescreenfloorplanbuttoncolor
                radius: style.but_smooth
                buttontext: ""

                Image {
                    id: onimg
                    source: "../../../img/icons/jabber_protocol.png"
                    height: parent.height
                    width: parent.width
                    anchors.centerIn: parent

                }
                MouseArea{
                    id: mousearea1
                    anchors.fill: parent
                    onClicked: showfloorplan(2)

                }

            }
        Flickable{
            height: scaleY(15)
            width: scaleX(100)
            anchors.left: lightingfloorplan.right
            anchors.leftMargin: 4
            contentHeight: style.buttonH
            contentWidth: ((style.buttonW + 5) * (lightingScenarios.count + 1)) - 5
            clip: true
            flickableDirection: "HorizontalFlick"



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
}

