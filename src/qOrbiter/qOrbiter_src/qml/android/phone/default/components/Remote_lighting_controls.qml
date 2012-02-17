import QtQuick 1.0

Column {
    width: 45
    height: 45
    spacing: 5

    Rectangle{
        width: 45
        height: 45
        color:"transparent"

        Image {
            id: up_bg
            source: "../img/buttonbg.png"
            anchors.fill:parent
        }
        Image {
            id: hoverimgup
            source: "../img/buttonhover.png"
            visible: false
            anchors.fill: parent
        }
        Image {
            id: image2            
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
            anchors.fill: parent
            source: "../img/On.png"
        }
    }

    Rectangle{
        width: 45
        height: 45
        color:"transparent"
        Image {
            id: dn_bg
            source: "../img/buttonbg.png"
            anchors.fill: parent
        }
        Image {
            id: hoverimg
            source: "../img/buttonhover.png"
            visible: false
            anchors.fill: parent
        }
        Image {
            id: ltdn
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
