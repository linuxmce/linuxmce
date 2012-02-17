import QtQuick 1.0

Column {
    width: 45
    height: childrenRect.height
    spacing: 5

    Rectangle{
        width: 45
        height: 45
        color:"transparent"
        Image {
            id: hoverimg
            source: "../img/buttonhover.png"
            visible: false
            anchors.fill: parent
        }
        Image {
            id: up_bg
            source: "../img/buttonbg.png"
            anchors.centerIn: parent
            anchors.fill: parent
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
            onPressAndHold: dcerouter.adjustVolume(1)
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
            anchors.centerIn: parent
            width: 45
            height: 45
            source: "../img/kmix.png"
        }
        MouseArea{
            anchors.fill: parent
            onClicked: dcerouter.mute()
        }
    }

    Rectangle{
        width: 45
        height: 45
        color:"transparent"

        Image {
            id: dn_bg
            source: "../img/buttonbg.png"
            anchors.centerIn: parent
        }
        Image {
            id: hoverimg2
            source: "../img/buttonhover.png"
            visible: false
            anchors.fill: parent
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
            onPressAndHold: dcerouter.adjustVolume(-1)
            onPressed: hoverimg2.visible = true
            onReleased: hoverimg2.visible = false
        }
    }









}
