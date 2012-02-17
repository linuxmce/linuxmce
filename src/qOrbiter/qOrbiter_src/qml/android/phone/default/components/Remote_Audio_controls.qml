import QtQuick 1.0

Column {
    height: childrenRect.height
    width:50
    spacing: 5

    Rectangle{
        height: 50
        width:50
        color:"transparent"

        Image {
            id: up_bg
            height: 50
            width:50
            source: "../img/buttonbg.png"
            anchors.centerIn: parent
            anchors.fill: parent
        }
        Image {
            id: hoverimg
            height: 50
            width:50
            source: "../img/buttonhover.png"
            visible: false
            anchors.fill: parent
        }
        Image {
            id: image2
            height: 50
            width:50
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
            height: 50
            width:50
            anchors.fill: parent
            source: "../img/kmix.png"
        }
        MouseArea{
            anchors.fill: parent
            onClicked: dcerouter.mute()
        }
    }

    Rectangle{
        height: 50
        width:50
        color:"transparent"

        Image {
            id: dn_bg
            height: 50
            width:50
            source: "../img/buttonbg.png"
            anchors.centerIn: parent
        }
        Image {
            id: hoverimg2
            height: 50
            width:50
            source: "../img/buttonhover.png"
            visible: false
            anchors.fill: parent
        }

        Image {
            id: image1
            height: 50
            width:50
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
