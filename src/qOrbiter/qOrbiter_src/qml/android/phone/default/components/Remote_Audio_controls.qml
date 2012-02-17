import QtQuick 1.0

Column {
    height: childrenRect.height
    width:45
    spacing: 5


    Rectangle{
        height: 45
        width:45
        color:"transparent"

        Image {
            id: up_bg
            height: 45
            width:45
            source: "../img/buttonbg.png"
            anchors.centerIn: parent

        }
        Image {
            id: hoverimg
            height: 45
            width:45
            source: "../img/buttonhover.png"
            visible: false

        }

        Image {
            id: image2
            height: 45
            width:45
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
            height: 45
            width:45
            anchors.fill: parent
            source: "../img/kmix.png"
        }
        MouseArea{
            anchors.fill: parent
            onClicked: dcerouter.mute()
        }
    }

    Rectangle{
        height: 45
        width:45
        color:"transparent"

        Image {
            id: dn_bg
            height: 45
            width:45
            source: "../img/buttonbg.png"
            anchors.centerIn: parent
        }
        Image {
            id: hoverimg2
            height: 45
            width:45
            source: "../img/buttonhover.png"
            visible: false

        }

        Image {
            id: image1
            height: 45
            width:45
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
