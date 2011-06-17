import QtQuick 1.0

Rectangle {
    id: remoteControl

    width: 800
    height: 600
    radius: 0
    opacity: 1
    color: "brown"

    Rectangle {
        id:video_playlist
        height: remoteControl.height -10
        width: 200
        color: "darkseagreen"
        anchors.left: parent.left;

        anchors.margins: 10

        Flickable {
            id: flickable1
            anchors.fill: parent

            Repeater{model: 50;
                ButtonSq{id:index; buttontext:index}
                   }

              }
    }

    //main 'now playing rect containing all the other items
    Rectangle{
        id:video_now_playing
        height: 200
        width: 200
        anchors.left: video_playlist.right
        anchors.leftMargin: 10

        color: "sienna"
        Text {
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
            Text {
                id: now_playing_label
                text: "Now Playing"
                anchors.top: parent.top
            }
        }
    }

    Rectangle{
        id:media_control_grp
        x: 425
        y: 200
        width: 375
        height: 328
        anchors.rightMargin: -430
        anchors.right: remote_lighting_controls1.left

        Roundbuttonbase{id:play_button; rnd_but_label: "Play!"; height: 50; anchors.horizontalCenter: parent.horizontalCenter; anchors.verticalCenter: parent.verticalCenter; width: 50; }
        Arrow{id:ff; x: 227; y: 152; width: 50; height: 25; arrow_label: "Fast Forward"; color: "green"; anchors.verticalCenterOffset: 1; anchors.verticalCenter: play_button.verticalCenter }
        Arrow{id:rw; x: 104; y: 152; height: 25; arrow_label: "Rewind"; color: "yellow"; anchors.verticalCenterOffset: 1; anchors.verticalCenter: parent.verticalCenter }

        ButtonSq { // not im using different base elements to accomplish the same goal
            id: stop
            x: 163
            y: 97
            width: 52
            height: 25

            Text {
                id: txt_stop
                x: 12
                y: 5
                text: "Stop"
                font.pixelSize: 12
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
}
