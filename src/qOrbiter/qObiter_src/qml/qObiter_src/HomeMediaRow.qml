import QtQuick 1.0

Rectangle {
    signal showDatagrid( string msg)
    signal gotoScreen( string screen)    
    property alias nowplayingtext:now_playing.buttontext
    property alias rowlabel: rowheader.buttontext

    height: 80
    width: parent.width
color:style.rowbgColor

Flickable{
    id:mediaflick
    interactive: true
    height: 80
    width: parent.width
    contentHeight: 80
    contentWidth: childrenRect * 1
    clip: true

    Row {
        id: guide
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        spacing:5


        ButtonSq {
            id: rowheader
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "Media"

            }

        ButtonSq {
            id: now_playing
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: ""

            }
        ButtonSq {
            id: mythTV
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "MythTV"


            }

        ButtonSq {
            id: videos
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "Videos"

            MouseArea{
                anchors.fill: parent
                onClicked: dceObject.requestDataGrid("5||||1,2|0|13|0 | 2 |")
            }


            }
        ButtonSq {
            id: audio
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "Audio"

            MouseArea {
                anchors.fill: parent
                onClicked: dceObject.requestDataGrid("4||||1,2|0|13|0 | 2 |")
            }

            }
        ButtonSq {
            id: games
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "LinuxMCE Games"

            }
        ButtonSq {
            id: hulu
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "Hulu Dekstop"

            }

        ButtonSq {
            id: playlists
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "Playlists"

            }
        ButtonSq {
            id: drives
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "Manage Drives"
            }
        ButtonSq {
            id: tv
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "DirecTv"

            }
        ButtonSq {
            id: ps3
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: "PlayStation 3"
            }
         }
    }
}
