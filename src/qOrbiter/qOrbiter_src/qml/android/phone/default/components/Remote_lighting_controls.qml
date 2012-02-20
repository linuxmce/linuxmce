import QtQuick 1.0

Column {
    id:lightingColumn
    width: style.stdbuttonw
    height: (style.stdbuttonH * 3) + (lightingColumn.spacing *3)
    spacing: scaleY(1)

    Rectangle{
        height: style.stdbuttonh
        width:style.stdbuttonw
        color:"transparent"

        Image {
            id: up_bg
            anchors.fill: parent
            source: "../img/buttonbg.png"
        }
        Image {
            id: hoverimgup
            source: "../img/buttonhover.png"
            anchors.fill: parent
            visible: false

        }
        Image {
            id: image2
            smooth: true
            anchors.centerIn: parent
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
        Image{
            id: center_bg
            source:"../img/chkbxempty.png"
            height: style.stdbuttonh
            width:style.stdbuttonw
        }

        Image {
            id: lightimg
           anchors.centerIn: parent
            anchors.fill: parent
            source: "../img/On.png"
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
            id: hoverimg
            height: style.stdbuttonh
            width:style.stdbuttonw
            source: "../img/buttonhover.png"
            visible: false

        }
        Image {
            id: ltdn
            anchors.centerIn: parent
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
