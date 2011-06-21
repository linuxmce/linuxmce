import QtQuick 1.0

Rectangle {
    width: 225
    height: 225
    color: style.advanced_bg

    MediaButton {id:play ; x: 88; y: 88;media_but_txt: "Play" }

    MediaButton {id:ff ; x: 168; y: 88;media_but_txt: "FF" }

    MediaButton {id:rw ; x: 10; y: 88;media_but_txt: "RW" }

    MediaButton {id:next ; x: 88; y: 11; media_but_txt: "Next"; }

    MediaButton {id:back ; x: 88; y: 169;media_but_txt: "back" }
}
