import QtQuick 1.1

Rectangle {
    width: childrenRect.width
    height: childrenRect.height
    color: "transparent"


    Row{
        spacing: 5

        MediaButton {
            id:back
            color: "transparent"
            media_but_txt: "|<"
            button.onClicked:  manager.newTrack("-1")
        }

        MediaButton {id:rw
            color: "transparent"
            media_but_txt: "<<"
            button.onClicked: manager.setPlaybackSpeed(-2)
        }

        MediaButton {
            id:pause
            color: "transparent"
            media_but_txt: "||"
            button.onClicked:  manager.pauseMedia()
        }

        MediaButton {
            id:play
            color: "transparent"
            media_but_txt: ">"
            button.onClicked:  manager.mythTvPlay()
        }

        MediaButton {
            id:ff
            color: "transparent"
            media_but_txt: ">>"
            button.onClicked: manager.setPlaybackSpeed(+2)
        }

        MediaButton {id:next ;
            color: "transparent"
            media_but_txt: ">|";
            button.onClicked: manager.newTrack("+1")
        }


        MediaButton {
            id: stop
            color: "transparent"
            media_but_txt: "[]"
            button.onClicked:  manager.stopMedia()
        }
    }
}
