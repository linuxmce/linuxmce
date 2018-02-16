import QtQuick 2.2
import "../components"
import "../"
import org.linuxmce.screens 1.0
Item{
    id:now_playing_button
    anchors{
        top:parent.top
        left:parent.left
        right:parent.right
    }
    clip:true
    height: dcenowplaying.b_mediaPlaying ?  appStyle.appNavigation_panelHeight : 0
    Rectangle{
        anchors.fill: parent
        color:appStyle.appbutton_cancel_color
    }

    StyledText{
        anchors{
            left:parent.left
            verticalCenter: parent.verticalCenter

        }
         text:qsTr("Now Playing:")
         font.bold: true
    }

//    Image {
//        id: now_playing_image
//        fillMode: Image.PreserveAspectFit
//        source: "image://listprovider/updateobject/"+securityvideo.timestamp
//        anchors.left: parent.left
//    }

    StyledText{
        id:now_playing_text
        text:  dcenowplaying.mediatitle === "" ? dcenowplaying.qs_mainTitle : dcenowplaying.mediatitle
        anchors{
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }
        font.weight: Font.DemiBold
    }

    StyledText{
        id:time
        font.bold: true
        text: dcenowplaying.b_mediaPlaying ?   roomList.currentEaTimecode.currentTimeCode : "00:00:00.000"
        anchors{
            right: parent.right
            verticalCenter: parent.verticalCenter
        }
    }

    MouseArea{
        anchors.fill: parent
        onClicked: {
            manager.setCurrentScreen(dcenowplaying.qs_screen)
        }
    }
}
