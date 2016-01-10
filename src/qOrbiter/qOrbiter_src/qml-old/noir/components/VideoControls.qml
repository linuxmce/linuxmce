import QtQuick 2.0

Rectangle {
    width: childrenRect.width
    height: childrenRect.height
    color: "transparent"

    Row{
        spacing: 5

        MediaButton {
            id:back
            color: "transparent"
            media_but_txt: "back"
            Image {
                id: backicon
                source: "../img/external/icons/player_end.png"
                rotation: 180
                height: parent.height
                width: parent.width             
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: back.scale = .75
                onExited: back.scale = 1
                onClicked: manager.changedTrack("-1")
            }
        }

        MediaButton {id:rw ;
            color: "transparent"
            media_but_txt: "RW"
            Image {
                id: rwicon
                source: "../img/external/icons/player_rew.png"
                height: parent.height
                width: parent.width
            }
            MouseArea{
                anchors.fill: parent
                 hoverEnabled: true
                onEntered: parent.scale = .75
                onExited: parent.scale = 1
                onClicked: manager.RwMedia(-2)
            }
        }

        MediaButton {
            id:play ;
            color: "transparent"
            media_but_txt: "Play"
            Image {
                id: playicon
                source: "../img/external/icons/player_play.png"
                height: parent.height
                width: parent.width

            }
            MouseArea {
                anchors.fill: parent
                 hoverEnabled: true
                onEntered: parent.scale = .75
                onExited: parent.scale = 1
                onClicked: manager.PauseMedia()
            }
        }

        MediaButton {id:ff ;
            color: "transparent"
            media_but_txt: "FF"
            Image {
                id: fficon
                source: "../img/external/icons/player_fwd.png"
                height: parent.height
                width: parent.width
            }
            MouseArea{
                anchors.fill: parent
                 hoverEnabled: true
                onEntered: parent.scale = .75
                onExited: parent.scale = 1
                onClicked: manager.FfMedia(2)
            }

        }
        MediaButton {id:next ;
            color: "transparent"
            media_but_txt: "Next";
            Image {
                id: nexticon
                source: "../img/external/icons/player_end.png"
                height: parent.height
                width: parent.width
            }
            MouseArea{
                anchors.fill: parent
                 hoverEnabled: true
                onEntered: parent.scale = .75
                onExited: parent.scale = 1
                onClicked: manager.changedTrack("+1")
            }
        }


        MediaButton {
            id: stop
            color: "transparent"
            media_but_txt: "stop"
            Image {
                id: stopicon
                source: "../img/external/icons/player_stop.png"
                height: parent.height
                width: parent.width
            }
            MouseArea
            {
                anchors.fill: parent
                 hoverEnabled: true
                onEntered: parent.scale = .75
                onExited: parent.scale = 1
                onClicked: manager.StopMedia()
            }
        }
    }
}
