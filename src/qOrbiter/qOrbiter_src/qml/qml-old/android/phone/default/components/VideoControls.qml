import QtQuick 1.0

Rectangle {
    width: parent.width
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
                source: "../images/player_end.png"
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
                source: "../images/player_rew.png"
                height: parent.height
                width: parent.width
            }
            MouseArea{
                anchors.fill: parent
                 hoverEnabled: true
                onEntered: parent.scale = .75
                onExited: parent.scale = 1
                onClicked: rw_media(-2)
            }
        }

        MediaButton {
            id:play ;
            color: "transparent"
            media_but_txt: "Play"
            Image {
                id: playicon
                source: "../images/player_play.png"
                height: parent.height
                width: parent.width

            }
            MouseArea {
                anchors.fill: parent
                 hoverEnabled: true
                onEntered: parent.scale = .75
                onExited: parent.scale = 1
                onClicked: pauseMedia()
            }
        }

        MediaButton {id:ff ;
            color: "transparent"
            media_but_txt: "FF"
            Image {
                id: fficon
                source: "../images/player_fwd.png"
                height: parent.height
                width: parent.width
            }
            MouseArea{
                anchors.fill: parent
                 hoverEnabled: true
                onEntered: parent.scale = .75
                onExited: parent.scale = 1
                onClicked: ff_media(2)
            }

        }
        MediaButton {id:next ;
            color: "transparent"
            media_but_txt: "Next";
            Image {
                id: nexticon
                source: "../images/player_end.png"
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
                source: "../images/player_stop.png"
                height: parent.height
                width: parent.width
            }
            MouseArea
            {
                anchors.fill: parent
                 hoverEnabled: true
                onEntered: parent.scale = .75
                onExited: parent.scale = 1
                onClicked: stopMedia()
            }
        }
    }
}
