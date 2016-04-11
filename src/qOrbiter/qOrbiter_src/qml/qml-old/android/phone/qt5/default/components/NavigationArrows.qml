import QtQuick 2.2

Rectangle {
    id:navigation
    width: childrenRect.width
    height: childrenRect.height
    color: "transparent"

    Row{
        spacing: 5


        MediaButton {id:left ;
            color: "transparent"
            media_but_txt: "Left"
            Image {
                id: rwicon
               source: "../images/navarrow.png"
                height: parent.height
                width: parent.width
                rotation: 180
            }
            MouseArea{
                anchors.fill: parent
                 hoverEnabled: true
                onEntered: parent.scale = .75
                onExited: parent.scale = 1
                onClicked: moveDirection("left")
            }
        }

        MediaButton {
            id:up ;
            color: "transparent"
            media_but_txt: "Up"
            Image {
                id: playicon
               source: "../images/navarrow.png"
                height: parent.height
                width: parent.width
                rotation: 270

            }
            MouseArea {
                anchors.fill: parent
                 hoverEnabled: true
                onEntered: parent.scale = .75
                onExited: parent.scale = 1
                onClicked: moveDirection("up")
            }
        }

        MediaButton {id:dwn ;
            color: "transparent"
            Image {
                id: fficon
                source: "../images/navarrow.png"
                height: parent.height
                width: parent.width
                rotation: 90
            }
            media_but_txt: "Down"

            MouseArea{
                anchors.fill: parent
                 hoverEnabled: true
                onEntered: parent.scale = .75
                onExited: parent.scale = 1
                onClicked: moveDirection("down")
            }

        }
        MediaButton {id:right ;
            color: "transparent"
            media_but_txt: "right";
            Image {
                id: nexticon
            source: "../images/navarrow.png"
                height: parent.height
                width: parent.width
            }
            MouseArea{
                anchors.fill: parent
                 hoverEnabled: true
                onEntered: parent.scale = .75
                onExited: parent.scale = 1
                onClicked: moveDirection("right")
            }
        }


        AvOptionButton {
            id: stop
            color: "transparent"
           buttontext:  "Enter"

            MouseArea
            {
                anchors.fill: parent
                 hoverEnabled: true
                onEntered: parent.scale = .75
                onExited: parent.scale = 1
                onClicked: moveDirection("enter")
            }
        }
    }
}
