import QtQuick 1.0
import "../js/ComponentLoader.js" as MyJs
Rectangle {


    width: style.orbiterW
    height: scaleY(9)
    color: "#9e9d9d"
    id:advanced_panel

    Row{
        id:advancedrow
        height:scaleY(9)
        width: childrenRect.width
        anchors.centerIn: parent
        anchors.horizontalCenter: parent.horizontalCenter
        spacing:10
        clip: false

        ButtonSq {
            id: buttonsq1

            width: scaleX(16)
            height: scaleY(9)
            color: style.homescreenfloorplanbuttoncolor
            buttontextfontsize: 9
            border.width: 2
            buttontext: "Advanced"
            radius: 5

            MouseArea{
                anchors.fill: parent
                onClicked:gotoQScreen("Screen_44.qml")
            }
        }

        ButtonSq {
            id: buttonsq2
            x: 63
            y: 1
            width: scaleX(16)
            height: scaleY(9)
            color: style.homescreenfloorplanbuttoncolor
            border.width: 2
            buttontext: "Power"

            radius: 5
            MouseArea{
                anchors.fill: parent
                onClicked:MyJs.createPowerButton()
            }
        }


    ButtonSq {
        id: buttonsq3
        x: 125
        y: 1
        width: scaleX(16)
        height: scaleY(9)
        color: style.homescreenfloorplanbuttoncolor
        border.width: 2
        buttontext: "Sleeping Menu"
        radius: 5
    }



    ButtonSq{
        id:roombutton
        buttontext: currentroom
        width: scaleX(16)
        height: scaleY(9)
        color: style.homescreenfloorplanbuttoncolor
        MouseArea{
            anchors.fill: parent
            onClicked:MyJs.createRoomSelector()
        }
    }

    UserListComponent {id:user}

    ButtonSq{
        id:exit
        buttontext: "Exit Orbiter"
        width: scaleX(16)
        height: scaleY(9)
        color: style.homescreenfloorplanbuttoncolor
        MouseArea{
            anchors.fill: exit
            onClicked: closeOrbiter()
        }
    }

}
}

