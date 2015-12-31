import QtQuick 2.0
import "../components"
import "../effects"
//import Qt.labs.shaders 1.0


Item
{
    anchors.centerIn: parent

//    Component.onCompleted:  {
//        if(!dceplayer.connected){
//        console.log("Starting media player")
//        dceplayer.setConnectionDetails(manager.qMediaPlayerID, manager.dceIP)
//        }
//    }


    Rectangle {
        id:stage

        signal swapStyle()
        height: manager.appHeight
        width: manager.appWidth
        color: "transparent"

        Image {
            id: headerbg
            source: "../img/ui3/header.png"
            anchors.top: stage.top
            height: scaleY(7)
            width: scaleX(99)

        }

        StyledText{
            id:connectstatus
            text: "Orbiter "+ manager.iPK_Device + " is connected."
            color: "aliceblue"
            font.letterSpacing: 2
            font.family: keyFont.name
            anchors.left: parent.left
            anchors.leftMargin: scaleX(5)
            anchors.verticalCenter: headerbg.verticalCenter

        }

        Clock{
            id:screen1time
            anchors.right: headerbg.right
            anchors.rightMargin: scaleX(25)
            anchors.verticalCenter: headerbg.verticalCenter
        }

        Column{
            id:maindisplay
            anchors.top:headerbg.bottom
            height: childrenRect.height
            width: scaleX(100)
            HomeLightingRow{id:lightRow }
            HomeMediaRow{id: mediaRow}
            HomeClimateRow{id: climateRow}
            HomeTelecomRow{id: telecomRow}
            HomeSecurityRow{id: securityRom}

        }
        BottomPanel{id: advanced; anchors.top: maindisplay.bottom}
    }
}

