import QtQuick 1.1
import "../../../skins-common/lib/handlers"
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
          ArrowLeftHandler{}
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
           ArrowUpHandler{autoTick: 750}
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

           ArrowDownHander{}

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
           ArrowRightHandler{}
        }


        AvOptionButton {
            id: stop
            color: "transparent"
           buttontext:  "Enter"

          EnterButtonHandler{}
        }
    }
}
