import QtQuick 1.0

Rectangle {

    width: 720
    height: 480
    color: "lightsteelblue"
    Text {
        id: orbiter_status
        text: "Orbiter Needs to regen - Click 'Later' because regen isnt implemented. Kthxbai"
    }

    Rectangle{
        height: 200
        width: 200

        color: "navajowhite"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        ButtonSq{
            radius: 2
            buttontext: "Regen Now"
            anchors.left: parent.left
            MouseArea{

            }
        }

        ButtonSq{
            x: 41
            y: 80
            radius: 2
            buttontext: "Later"
            anchors.right: parent.right
            MouseArea{
                 anchors.fill: parent
                onClicked: dceObject.gotoQScreen("Screen_1.qml")
            }
        }
    }

}
