import QtQuick 1.0

Rectangle {
    width: 300
    height: 300
    color: style.advanced_bg

    MediaButton {id:play ; x: 125; y: 125;media_but_txt: "Play" }

    MediaButton {id:ff ; x: 209; y: 125;media_but_txt: "FF" }

    MediaButton {id:rw ; x: 46; y: 125;media_but_txt: "RW" }

    MediaButton {id:next ; x: 125; y: 40; media_but_txt: "Next"; }

    MediaButton {id:back ; x: 125; y: 203;media_but_txt: "back" }
}
