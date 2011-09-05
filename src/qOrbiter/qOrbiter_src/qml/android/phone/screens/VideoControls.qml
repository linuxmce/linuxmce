import QtQuick 1.0

Rectangle {
    width: 225
    height: 225
    color: style.advanced_bg

    MediaButton {
        id:play ;
        x: 75; y: 75
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
        x: 155; y: 75
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

    MediaButton {id:rw ;
        x: -3; y: 75
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

    MediaButton {id:next ;
        x: 75; y: -2
        media_but_txt: "Next";
        Image {
            id: nexticon
            source: "../../../img/icons/player_end.png"
            height: parent.height
            width: parent.width
         }

    }

    MediaButton {id:back ;
        x: 75
        y: 156
        media_but_txt: "back"
        Image {
            id: backicon
            source: "../../../img/icons/player_start.png"
            height: parent.height
            width: parent.width
         }

    }

    MediaButton {
        id: mediabutton1
        x: 0
        y: 150
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
