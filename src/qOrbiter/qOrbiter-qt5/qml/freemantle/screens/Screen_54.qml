import QtQuick 1.0
import "../components"

Rectangle {
    property alias videoTitle: video_title.text
   // property alias synText:
    id: pvrRemote
    Style{id:style}

    height: style.orbiterH
    width: style.orbiterW
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
    Remote_lighting_controls{ id: remote_lighting_controls1; x: 331; y: 181; width: 93; height: 219; anchors.topMargin: 179;anchors.top: video_now_playing.baseline}
    Remote_Audio_controls{ id: remote1; x: 321; y: 194; z: 45; anchors.right: remote_lighting_controls1.left}

    ButtonSq {
        id: buttonsq1
        x: 752
        y: 0
        width: 50
        height: 50

        buttontext: "Home"

        Image {
            id: homeimg
            height: parent.height
            width: parent.width
            source: "../../img/icons/agt_home.png"

        }
        MouseArea {
            id: mouse_area1
            anchors.fill: parent
            onClicked: changeScreen("Screen_1.qml")
        }
    }

    VideoControls {
        id: videocontrols1
        x: 537
        y: 231
        width: 263
        height: 249
    }

    Column {
        id: channelgrid

        width: 200
        height: 500
        clip: true

        Flickable{
            height: parent.height
            width: parent.width
            contentHeight: childrenRect.width
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
