import QtQuick 1.0

Column {
    width: style.buttonw
    height: childrenRect.height
    spacing: 5

    Rectangle{
        height: style.buttonh
        width: style.buttonw
        color:"transparent"

        Image {
            id: up_bg
            source: "../img/buttonbg.png"
            anchors.centerIn: parent
        }
        Image {
            id: hoverimgup
            source: "../img/buttonhover.png"
            visible: false
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
            onClicked: dcerouter.adjustLighting(10)
            onPressed: hoverimgup.visible = true
            onReleased: hoverimgup.visible = false
        }
    }



    ButtonSq{

        anchors.centerIn: parent
        buttontext: ""
        buttontextzindex: 20
        buttontextcolor: "red"
        buttontextbold: true
        buttontextfontsize: scaleY(2)

        Image {
            id: lightimg
            anchors.centerIn: parent
            height: style.buttonh
            width: style.buttonw
            source: "../img/On.png"
        }
    }

    Rectangle{
        height: style.buttonh
        width: style.buttonw
        color:"transparent"
        Image {
            id: dn_bg
            source: "../img/buttonbg.png"
            anchors.centerIn: parent
        }
        Image {
            id: hoverimg
            source: "../img/buttonhover.png"
            visible: false
            anchors.fill: parent
        }
        Image {
            id: ltdn
            anchors.centerIn: parent
            smooth: true
            source: "../img/minus.png"

        }
        MouseArea{
            anchors.fill: parent
            onPressAndHold: dcerouter.adjustLighting(-10)
            onClicked: dcerouter.adjustLighting(-10)
            onPressed: hoverimg.visible = true
            onReleased: hoverimg.visible = false
        }

    }



}
