import QtQuick 1.0

Item{
    id:nowplayingbutton
    visible: dcenowplaying.b_mediaPlaying ? true : false
    width:!visible ? 0 :scaleX(20)
    height:scaleY(16)

    clip:true

    Rectangle{
        anchors.fill: parent
        color:ms.pressed ? "white" : "darkblue"
        opacity: ms.pressed ? 1 : .25
        border.color: "white"
        border.width: 2

        Behavior on color{
            PropertyAnimation{
                duration: 500
            }
        }

        Behavior on opacity {
            PropertyAnimation{
                duration:500
            }
        }
    }

    StyledText{
        id:sT
        text:dcenowplaying.qs_mainTitle
        fontSize: scaleY(3.5)
        anchors.top: parent.top
        isBold: true
        height: parent.height-tm.height
        color:"white"
        width: parent.width
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        elide:Text.ElideRight
    }
    StyledText{
        id:tm
        text: dceTimecode.qsCurrentTime
        fontSize: scaleY(3.5)
        anchors.bottom: parent.bottom
        isBold: true
        color:"white"
    }

    MouseArea{
        id:ms
        anchors.fill: parent
       onClicked:manager.setCurrentScreen(dcenowplaying.qs_screen)
    }
}


