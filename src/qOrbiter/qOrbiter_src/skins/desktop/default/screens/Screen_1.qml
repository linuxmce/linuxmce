import QtQuick 1.0
import "../components"


    Rectangle {
        id:stage
anchors.centerIn: parent
        signal swapStyle()
        height: style.orbiterH
        width: style.orbiterW
        color: "transparent"
        Image {
            id: bg
            source: "../img/bkg.png"
            anchors.fill: parent
        }



        Column{
            id:maindisplay
            anchors.centerIn: parent
            height: childrenRect.height
            width: scaleX(100)
            NowPlayingBox{anchors.horizontalCenter: parent.horizontalCenter}

            DroidHomeSelector{anchors.horizontalCenter: parent.horizontalCenter}


        }

    }


