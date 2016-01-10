import QtQuick 1.0
Item {


    height: style.homescreenrowheight
    width: parent.width

    HomeButtonDelegate{id:lightingdelegate}
    Flickable{
        height: style.buttonH
        width: parent.width
        contentHeight: style.buttonH
        contentWidth: childrenRect.width*2
        clip: true

        Row {
        id: guide
        //anchors.left: lightingfloorplan.right
        spacing:5

        ButtonSq {
            id: lightingfloorplan
            height: style.buttonH
            width: style.buttonW
            color: style.homescreenfloorplanbuttoncolor
            radius: style.but_smooth
            buttontext: ""

            Image {
                id: onimg
                source: "../../../img/icons/jabber_protocol.png"
                height: style.iconHeight
                width: style.iconWidth
                anchors.centerIn: parent

                    }
            MouseArea{
                id: mousearea1
                anchors.fill: parent
                onClicked: setCurrentScreen("Screen_2.qml")

              }

        }

        ListView{
            id: lightingScenarios
            width: stage.width
            height: style.buttonH
            model: currentRoomLights
            spacing: 5
            orientation:ListView.Horizontal
            delegate:  lightingdelegate
            interactive: false
                 }
        }
    }
}

