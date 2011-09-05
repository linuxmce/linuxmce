import QtQuick 1.0

Rectangle {
    width: 500
    height: 100
    color: "transparent"
    Row{
        anchors.centerIn: parent

        MediaButton {id:back ;

            media_but_txt: "back"
            Image {
                id: backicon
               source: "../../../img/icons/player_end.png"
              rotation: 180
                height: parent.height
                width: parent.width
            }

        }

        MediaButton {id:rw ;

            media_but_txt: "RW"
            Image {
                id: rwicon
                source: "../../../img/icons/player_rew.png"
                height: parent.height
                width: parent.width
            }
            MouseArea{
                anchors.fill: parent
                onClicked: rw_media(-2)
            }
        }

        MediaButton {
            id:play ;

            media_but_txt: "Play"
            Image {
                id: playicon
                source: "../../../img/icons/player_play.png"
                height: parent.height
                width: parent.width

            }
            MouseArea {
                anchors.fill: parent
                onClicked: pauseMedia()
            }
        }

        MediaButton {id:ff ;

            media_but_txt: "FF"
            Image {
                id: fficon
                source: "../../../img/icons/player_fwd.png"
                height: parent.height
                width: parent.width
            }
            MouseArea{
                anchors.fill: parent
                onClicked: ff_media(2)
            }

        }
        MediaButton {id:next ;

            media_but_txt: "Next";
            Image {
                id: nexticon
                source: "../../../img/icons/player_end.png"
                height: parent.height
                width: parent.width
            }

        }


        MediaButton {
            id: stop

            color: "#c28585"
            media_but_txt: "stop"
            Image {
                id: stopicon
                source: "../../../img/icons/player_stop.png"
                height: parent.height
                width: parent.width
            }
            MouseArea
            {
                anchors.fill: parent
                onClicked: stopMedia()
            }
        }
    }
}
