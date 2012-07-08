import QtQuick 1.0

Column {
    id:audio_column
    height: (style.stdbuttonH * 3) + (audio_column.spacing *3)
    width:style.stdbuttonw
    spacing: scaleY(1)


    Rectangle{
        height: style.stdbuttonh
        width:style.stdbuttonw
        color:"transparent"

        Image {
            id: up_bg
            height: style.stdbuttonh
            width:style.stdbuttonw
            source: "../img/buttonbg.png"
            anchors.centerIn: parent

        }
        Image {
            id: hoverimg
            height: style.stdbuttonh
            width:style.stdbuttonw
            source: "../img/buttonhover.png"
            visible: false

        }

        Image {
            id: image2            
            anchors.centerIn: parent
            smooth: true
            source: "../img/plus.png"
        }
        MouseArea{
            anchors.fill: parent
            onClicked: dcerouter.adjustVolume(1)

            onPressed: hoverimg.visible = true
            onReleased: hoverimg.visible = false
        }
    }

    ButtonSq{

        anchors.centerIn: parent
        buttontext: "Mute"
        buttontextzindex: 20
        buttontextcolor: "red"        
        buttontextfontsize: scaleY(2)
        Image {
            id: muteimg
            height: style.stdbuttonh
            width:style.stdbuttonw
            anchors.fill: parent
            source: "../img/kmix.png"
        }
        MouseArea{
            anchors.fill: parent
            onClicked: dcerouter.mute()
        }
    }

    Rectangle{
        height: style.stdbuttonh
        width:style.stdbuttonw
        color:"transparent"

        Image {
            id: dn_bg
            height: style.stdbuttonh
            width:style.stdbuttonw
            source: "../img/buttonbg.png"
            anchors.centerIn: parent
        }
        Image {
            id: hoverimg2
            height: style.stdbuttonh
            width:style.stdbuttonw
            source: "../img/buttonhover.png"
            visible: false

        }

        Image {
            id: image1
            anchors.centerIn: parent
            smooth: true
            source: "../img/minus.png"
        }
        MouseArea{
            anchors.fill: parent
            onClicked: dcerouter.adjustVolume(-1)

            onPressed: hoverimg2.visible = true
            onReleased: hoverimg2.visible = false
        }
    }









}
