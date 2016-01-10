import QtQuick 1.1
import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../effects"


Column
{
    width: scaleX(30)
    spacing: 5
    height: childrenRect.height
    
    Text {
        id: np_device
        width: parent.width
        text: qsTr("Device: ")  + dcenowplaying.qs_mainTitle
        font.family: "Droid Sans"
        wrapMode: "NoWrap"
        font.bold: true
        smooth: true
        font.pixelSize: 1 * scaleY(2)
        color: "aliceblue"
    }
    
    
    Text {
        id: network_id
        wrapMode: "NoWrap"
        text: qsTr("Network: ") + dcenowplaying.channelID
        font.family: "Droid Sans"
        font.bold: true
        smooth: true
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: scaleY(2)
        color: "white"
    }
    
    
    Text {
        id: channel_id
        wrapMode: "NoWrap"
        text: qsTr("Channel: ") + dcenowplaying.channel
        font.family: "Droid Sans"
        font.bold: true
        smooth: true
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: scaleY(2)
        color: "white"
    }
    
    Text {
        id: program_title
        wrapMode: "NoWrap"
        text: qsTr("Program:") + dcenowplaying.tvProgram
        font.family: "Droid Sans"
        font.bold: true
        smooth: true
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: scaleY(2)
        color: "white"
    }
    
    Text {
        id: live_av
        wrapMode: "NoWrap"
        text: qsTr("ScreenShot Availible:") + manager.monitorAvailible
        font.family: "Droid Sans"
        font.bold: true
        smooth: true
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: scaleY(2)
        color: "white"
    }
}
