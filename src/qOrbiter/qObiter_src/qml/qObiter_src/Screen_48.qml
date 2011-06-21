import QtQuick 1.0

Rectangle {
    property alias videoTitle: video_title.text
   // property alias synText:
    id: pvrRemote
    Style1{id:style}

    width: 640
    height: 480
    radius: 0
    opacity: 1
    color: style.advanced_bg

    //main 'now playing rect containing all the other items
    Rectangle{
        id:video_now_playing
        x: 224
        y: 0
        height: 179
        width: 200
        anchors.left: pvrRemote.right
        anchors.leftMargin: 14

        color: style.button_system_color
        Text {
            id: video_title
            text: "text"
        }
        Rectangle{
            id: title_box
            width: video_now_playing.width
            height: 15
            color: style.rowbgColor
            opacity: 5
            anchors.top: parent.top
            Text {
                id: now_playing_label
                text: "Now Playing"
                wrapMode: Text.WordWrap
                anchors.top: parent.top
            }

            Text {
                id: text2
                x: 58
                y: 94
                text: "whats playing"
                clip: true
                horizontalAlignment: Text.AlignLeft
                wrapMode: Text.WordWrap
                font.pixelSize: 12
            }
        }
    }
    Remote_lighting_controls{ id: remote_lighting_controls1; x: 331; y: 181; width: 93; height: 219; anchors.topMargin: 181;anchors.top: video_now_playing.baseline}
    Remote_Audio_controls{y: 200; width: 107; height: 219; anchors.topMargin: 2; x: 224; anchors.top: video_now_playing.bottom}

    ButtonSq {
        id: buttonsq1
        x: 752
        y: 0
        width: 48
        height: 25

        Text {
            id: text1
            x: 6
            y: 5
            text: "Home"
            font.pixelSize: 12
        }

        MouseArea {
            id: mouse_area1

            anchors.rightMargin: 0
            anchors.bottomMargin: 0
            anchors.leftMargin: 0
            anchors.topMargin: 0
            anchors.fill: parent
        }
    }

    VideoControls {
        id: videocontrols1
        x: 424
        y: 179
        width: 216
        height: 221
    }

    Column {
        id: channelgrid
        x: 0
        y: 0
        width: 200
        height: 400
        clip: true

        Flickable{
            Repeater { model: 50
                Rectangle {
                    width:200
                    height: 50
                    color: "whitesmoke"
                    Text {
                        text: "I am DG item ,Sroll me!"
                    }
                }
            }
}
    }
}
