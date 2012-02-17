import QtQuick 1.0

Column {
    width: 45
    height: childrenRect.height
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

        }
        Image {
            id: hoverimgup
            source: "../img/buttonhover.png"
            height: 45
            width:45
            visible: false

        }
        Image {
            id: image2
            height: 45
            width:45
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
        buttontextfontsize: 4

        Image {
            id: lightimg
            height: 45
            width:45
            anchors.fill: parent
            source: "../img/On.png"
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

        }
        Image {
            id: hoverimg
            height: 45
            width:45
            source: "../img/buttonhover.png"
            visible: false

        }
        Image {
            id: ltdn
            height: 45
            width:45
           anchors.fill: parent
            smooth: true
            source: "../img/minus.png"

        }
        MouseArea{
            anchors.fill: parent

            onClicked: dcerouter.adjustLighting(-10)
            onPressed: hoverimg.visible = true
            onReleased: hoverimg.visible = false
        }
    }
}
