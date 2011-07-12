import Qt 4.7

Rectangle {
    signal showDatagrid( string msg)
    signal gotoScreen( string screen, string type)
    property alias nowplayingtext:now_playing.buttontext
    property alias rowlabel: rowheader.buttontext

    height: 65
    width: parent.width
color:style.rowbgColor

Flickable{
    id:mediaflick
    interactive: true
    height: 65
    width: parent.width
    contentHeight: 65
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
            buttontext: ""

            Image {
                id: mHeaderImg
                source: "../../../img/icons/kmix.png"
                height: style.iconHeight
                width: style.iconWidth
                anchors.centerIn: parent                
                }           
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
            Image {
                id: mythIcon
                source: "../../../img/icons/Mythtv.png"
                height: style.iconHeight
                width: style.iconWidth
                anchors.centerIn: parent

            }
            buttontext:""


            }

        ButtonSq {
            id: videos
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth

            Image {
                id: videosIcon
                source: "../../../img/icons/xine.png"
                height: style.iconHeight
                width: style.iconWidth
                anchors.centerIn: parent

            }

            buttontext: "Videos"
            buttontextbold: true
            buttontextfontsize: 12
            buttontextcolor: "white"
            MouseArea{
                anchors.fill: parent
                onClicked: requestDataGrid("5||||1,2|0|13|0 | 2 |", "videos")
            }


            }
        ButtonSq {
            id: audio
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: ""
            Image {
                id: audioimg
                source: "/src/1004/src/qOrbiter/qObiter_src/img/icons/lsongs.png"
                height: style.iconHeight
                width: style.iconWidth
                anchors.centerIn: parent

            }
            MouseArea {
                anchors.fill: parent
                onClicked: requestDataGrid("4||||1,2|0|13|0 | 2 |", "music")
            }

            }
        ButtonSq {
            id: games
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth

            Image {
                id: gameimg
                source: "../../../img/icons/package_games.png"
                height: style.iconHeight
                width: style.iconWidth
                anchors.centerIn: parent

            }
            buttontext: ""
            MouseArea {
                anchors.fill: parent
                onClicked: requestDataGrid("4||||1,2|0|12|0 | 2 |", "games")
            }
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
            buttontext: ""
            Image {
                id: pls
                source: "../../../img/icons/kedit.png"
                height: style.iconHeight
                width: style.iconWidth
                anchors.centerIn: parent

            }

            }
        ButtonSq {
            id: drives
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: ""
            Image {
                id: drvimg
                source: "../../../img/icons/harddrive2.png"
                height: style.iconHeight
                width: style.iconWidth
                anchors.centerIn: parent

            }
            }


        ButtonSq {
            id: tv
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: ""

            Image {
                id: livetv
                source: "../../../img/icons/tv.png"
                height: style.iconHeight
                width: style.iconWidth
                anchors.centerIn: parent

            }

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
