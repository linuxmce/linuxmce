import QtQuick 1.0

Column {
    id:lightingColumn
    width: style.stdbuttonw
    height: style.stdbuttonh //(style.stdbuttonH * 3) + (lightingColumn.spacing *3)
    spacing: scaleY(1)

    Timer{
        id:closeTimer
        interval: 5000
        running: false
        onTriggered: lightingColumn.state = "DEFAULT"
    }

    states: [
        State {
            name: "expanded"
            PropertyChanges {
                target: lightingColumn
                height: (style.stdbuttonh * 3) + (lightingColumn.spacing *3)
            }
            PropertyChanges {
                target: lightingUp
                height: style.stdbuttonh
                width:style.stdbuttonw
                visible:true
            }
            PropertyChanges {
                target: lightingDown
                height: style.stdbuttonh
                width:style.stdbuttonw
                visible:true
            }
            PropertyChanges {
                target: floorplan
                height: style.stdbuttonh
                width:style.stdbuttonw
                visible:true
            }
            PropertyChanges{
                target: closeTimer
                running:true
            }
            PropertyChanges {
                target: triggerButton
                visible:false
                opacity:0

            }
        }
    ]

    Rectangle{
        id:lightingUp
        height: 0
        width:0
        color:"transparent"
        visible:false

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
            onClicked: manager.adjustLighting(10)
            onPressed: hoverimgup.visible = true
            onReleased:{ hoverimgup.visible = false; closeTimer.start()}
        }
    }

    ButtonSq{
        id:floorplan
        anchors.centerIn: parent
        buttontext: "F"
        buttontextzindex: 20
        buttontextcolor: "red"
        buttontextbold: true
        buttontextfontsize: 4
        height: 0
        width: 0
        visible:false
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
        MouseArea{
            anchors.fill: floorplan
            onClicked: manager.setCurrentScreen("Screen_2.qml")
        }
    }


    ButtonSq{
        id:triggerButton
        anchors.centerIn: parent
        buttontext: ""
        buttontextzindex: 20
        buttontextcolor: "red"
        buttontextbold: true
        buttontextfontsize: 4
        visible:true
        opacity:1
        Image {
           anchors.centerIn: parent
            anchors.fill: parent
            source: "../img/On.png"
            visible: triggerButton.visible

        }
        MouseArea{
            anchors.fill:parent
            onClicked: lightingColumn.state="expanded"
        }
    }

    Rectangle{
        id:lightingDown
        height: 0
        width:0
        color:"transparent"
        visible:false
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

            onClicked: manager.adjustLighting(-10)
            onPressed: hoverimg.visible = true
            onReleased: { hoverimg.visible = false; closeTimer.restart() }
        }
    }
}
