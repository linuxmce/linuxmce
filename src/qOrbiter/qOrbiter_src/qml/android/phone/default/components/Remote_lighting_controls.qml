import QtQuick 1.0

Column {
    width: style.iconWidth
    height: childrenRect.height
    spacing: 5

    Rectangle{
        height: style.iconHeight
        width: style.icongWidth
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
            anchors.centerIn: parent
            height: style.iconHeight
            width: style.icongWidth
            source: "../img/On.png"
        }
    }

    Rectangle{
        height: style.iconHeight
        width: style.icongWidth
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
            onPressAndHold: dcerouter.adjustLighting(-10)
            onClicked: dcerouter.adjustLighting(-10)
            onPressed: hoverimg.visible = true
            onReleased: hoverimg.visible = false
        }
    }
}
