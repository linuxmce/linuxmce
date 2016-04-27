import QtQuick 2.2

Column {
    id:audio_column
    height: style.stdbuttonh// (style.stdbuttonH * 3) + (audio_column.spacing *3)
    width:style.stdbuttonw
    spacing: scaleY(1)

    Timer{
        id:closeTimer
        interval: 5000
        onTriggered: audio_column.state = "DEFAULT"
        running:false
    }

    states: [
        State{
            name:"expanded"
            PropertyChanges {
                target: audio_column
                height:  (style.stdbuttonh * 3) + (audio_column.spacing *3)
            }
            PropertyChanges {
                target: volup
                height: style.stdbuttonh
                width:style.stdbuttonw
                visible:true

            }
            PropertyChanges {
                target: voldn
                height: style.stdbuttonh
                width:style.stdbuttonw
                visible:true

            }
            PropertyChanges {
                target: muteBtn
                height: style.stdbuttonh
                width:style.stdbuttonw
                visible:true

            }
            PropertyChanges {
                target: trigger
                height: 0
                width:0
                visible:false
            }
            PropertyChanges {
                target: closeTimer
                running:true
            }

        }

    ]


    Rectangle{
        id:volup
        height: 0
        width:0
        color:"transparent"
        visible: false
        Image {
            id: up_bg
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
            id: image2            
            anchors.centerIn: parent
            smooth: true
            source: "../img/plus.png"
        }
        MouseArea{
            anchors.fill: parent
            onClicked: manager.adjustVolume(1)
            onPressed: hoverimg.visible = true
            onReleased: { hoverimg.visible = false; closeTimer.restart()}
        }
    }

    ButtonSq{
        id:trigger
        anchors.centerIn: parent
        buttontext: "Options"
        buttontextzindex: 20
        buttontextcolor: "red"        
        buttontextfontsize: scaleY(2)
        visible:true
        Image {
            id: audioicn
            height: style.stdbuttonh
            width:style.stdbuttonw
            anchors.fill: parent
            source: "../img/kmix.png"
            visible:parent.visible
        }
        MouseArea{
            anchors.fill: parent
            onClicked: audio_column.state = "expanded"
        }
    }

    ButtonSq{
        id:muteBtn
        anchors.centerIn: parent
        buttontext: "Mute"
        buttontextzindex: 20
        buttontextcolor: "red"
        buttontextfontsize: scaleY(2)
        color: "red"
        visible: false

        Image {
            id: muteimg
            height: style.stdbuttonh
            width:style.stdbuttonw
            anchors.fill: parent
            source: "../img/kmix.png"
        }
        MouseArea{
            anchors.fill: parent
            onClicked: manager.mute()
        }
    }

    Rectangle{
        id:voldn
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
            id: hoverimg2
            height: style.stdbuttonh
            width:style.stdbuttonw
            source: "../img/buttonhover.png"
            visible: false

        }

        Image {
            id: image1
            anchors.centerIn: parent
            smooth: true
            source: "../img/minus.png"
        }
        MouseArea{
            anchors.fill: parent
            onClicked: manager.adjustVolume(-1)
            onPressed: hoverimg2.visible = true
            onReleased: {hoverimg2.visible = false; closeTimer.restart()}
        }
    }

}
