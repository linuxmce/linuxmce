import QtQuick 1.0

Rectangle {
    width: 225
    height: 225
    color: style.advanced_bg

    MediaButton {
        id:play ;
        x: 88; y: 88;
        media_but_txt: "Play"
        Image {
            id: playicon
            source: "../../../img/icons/player_play.png"
            height: parent.height
            width: parent.width

         }
    }

    MediaButton {id:ff ;
        x: 168; y: 88;
        media_but_txt: "FF"
        Image {
            id: fficon
            source: "../../../img/icons/player_fwd.png"
            height: parent.height
            width: parent.width
         }

    }

    MediaButton {id:rw ;
        x: 10; y: 88;
        media_but_txt: "RW"
        Image {
            id: rwicon
            source: "../../../img/icons/player_rew.png"
            height: parent.height
            width: parent.width
         }
    }

    MediaButton {id:next ;
        x: 88; y: 11;
        media_but_txt: "Next";
        Image {
            id: nexticon
            source: "../../../img/icons/player_end.png"
            height: parent.height
            width: parent.width
         }

    }

    MediaButton {id:back ;
        x: 88;
        y: 169;
        media_but_txt: "back"
        Image {
            id: backicon
            source: "../../../img/icons/player_start.png"
            height: parent.height
            width: parent.width
         }

    }
}
