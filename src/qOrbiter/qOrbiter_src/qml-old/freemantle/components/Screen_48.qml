import QtQuick 1.0
import "../components"

Rectangle {
    property alias videoTitle: video_title.text
   // property alias synText:
    id: pvrRemote
    Style{id:style}

    width: 800
    height: 480
    radius: 0
    opacity: 1
    color: style.advanced_bg

    //main 'now playing rect containing all the other items
    Rectangle{
        id:video_now_playing
        x: 200
        y: 2
        height: 179
        width: 224
        anchors.left: pvrRemote.right
        anchors.leftMargin: -600

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
                x: 0
                y: 0
                text: "Now Playing"
                anchors.topMargin: 0
                wrapMode: Text.WordWrap
                anchors.top: parent.top
            }

            Text {
                id: text2
                x: 139
                y: 139
                text: "whats playing"
                clip: true
                horizontalAlignment: Text.AlignLeft
                wrapMode: Text.WordWrap
                font.pixelSize: 12
            }
        }
    }
    Remote_lighting_controls{ id: remote_lighting_controls1; x: 331; y: 181; width: 65; height: 219; anchors.topMargin: 179;anchors.top: video_now_playing.baseline}
    Remote_Audio_controls{ id: remote1; x: 277; y: 181; width: 60; height: 219; anchors.rightMargin: -6; z: 45; anchors.right: remote_lighting_controls1.left}

    HomeButton{}


    VideoControls {
        id: videocontrols1
        x: 537
        y: 231
        width: 263
        height: 249
        color: "#e1e9f1"
    }

    Column {
        id: channelgrid

        width: 200
        height: 500
        clip: true

        Flickable{
            height: parent.height
            width: parent.width
            contentHeight: childrenRect.height
            contentWidth: childrenRect.width
            clip: true

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
