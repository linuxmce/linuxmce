import QtQuick 1.0

Column {
    width: 50
    height: childrenRect.height
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
            anchors.fill:parent
        }
        Image {
            id: hoverimgup
            source: "../img/buttonhover.png"
            height: 50
            width:50
            visible: false
            anchors.fill: parent
        }
        Image {
            id: image2
            height: 50
            width:50
            smooth: true
            anchors.fill: parent
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
            height: 50
            width:50
            anchors.fill: parent
            source: "../img/On.png"
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
            id: ltdn
            height: 50
            width:50
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
