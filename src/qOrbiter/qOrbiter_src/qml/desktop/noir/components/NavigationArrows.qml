import QtQuick 1.1

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
               source: "../img/icons/navarrow.png"
                height: parent.height
                width: parent.width
                rotation: 180
            }
            MouseArea{
                anchors.fill: parent
                 hoverEnabled: true
                onEntered: parent.scale = .75
                onExited: parent.scale = 1
                onClicked: manager.moveDirection("left")
            }
        }

        MediaButton {
            id:up ;
            color: "transparent"
            media_but_txt: "Up"
            Image {
                id: playicon
               source: "../img/icons/navarrow.png"
                height: parent.height
                width: parent.width
                rotation: 270

            }
            MouseArea {
                anchors.fill: parent
                 hoverEnabled: true
                onEntered: parent.scale = .75
                onExited: parent.scale = 1
                onClicked: manager.moveDirection("up")
            }
        }

        MediaButton {id:dwn ;
            color: "transparent"
            Image {
                id: fficon
                source: "../img/icons/navarrow.png"
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
                onClicked: manager.moveDirection("down")
            }

        }
        MediaButton {id:right ;
            color: "transparent"
            media_but_txt: "right";
            Image {
                id: nexticon
            source: "../img/icons/navarrow.png"
                height: parent.height
                width: parent.width
            }
            MouseArea{
                anchors.fill: parent
                 hoverEnabled: true
                onEntered: parent.scale = .75
                onExited: parent.scale = 1
                onClicked: manager.moveDirection("right")
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
                onClicked: manager.moveDirection("enter")
            }
        }
    }
}
