import QtQuick 2.0

Rectangle {
    id: remoteControl
    Style{id:style}

    width: 800
    height: 600
    radius: 0
    opacity: 1
    color: style.bgColor

    Rectangle {
        id:video_playlist
        height: remoteControl.height -10
        width: 200
        color: style.stage_bg
        anchors.left: parent.left;

        anchors.margins: 10

    }

    //main 'now playing rect containing all the other items
    Rectangle{
        id:video_now_playing
        height: 200
        width: 200
        anchors.left: video_playlist.right
        anchors.leftMargin: 10

        color: style.button_system_color
        StyledText {
            id: video_title
            text: "text"
        }
        Rectangle{
            id: title_box
            width: video_now_playing.width
            height: 15
            color: "darkorange"
            opacity: 5
            anchors.top: parent.top
            StyledText {
                id: now_playing_label
                text: "Now Playing"
                anchors.top: parent.top
            }
        }
    }
    Remote_lighting_controls{ id: remote_lighting_controls1; x: 370; y: 200; width: 50; height: 200; anchors.topMargin: 200;anchors.top: video_now_playing.baseline}
    Remote_Audio_controls{y: 200; width: 50; height: 200; anchors.topMargin: 0; x: 220; anchors.top: video_now_playing.bottom}

    ButtonSq {
        id: buttonsq1
        x: 752
        y: 0
        width: 48
        height: 25

        StyledText {
            id: text1
            x: 6
            y: 5
            text: "Home"
            font.pointSize: 12
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
        x: 492
        y: 300
    }
}
