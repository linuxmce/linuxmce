import QtQuick 1.0
import "../components"

Item {
    id: metadatavideo

    
    Image {
        id: npgloss
        source: "../img/widegreyshape.png"
        height:parent.height
        width:parent.width
        smooth:true
        opacity: .35
    }
    
    Column
    {
        id:textcol
        width: scaleX(45)
        spacing: 5
        height: childrenRect.height
        
        Text {
            id: artist
            width: parent.width
            text: qsTr("Device: ")  + dcenowplaying.qs_mainTitle
            font.family: "Droid Sans"
            wrapMode: "NoWrap"
            font.bold: true
            smooth: true
            font.pixelSize: scaleY(3)
            color: "orange"
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
    }
}
