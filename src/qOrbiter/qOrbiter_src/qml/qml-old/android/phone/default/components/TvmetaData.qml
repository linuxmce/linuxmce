import QtQuick 1.0
import "../components"

Item {
    id: metadatavideo

   Rectangle{
    anchors.fill: parent
    gradient:appstyle.bgContentGradient
   }
    
    Column
    {
        id:textcol
        width: scaleX(45)
        spacing: 5
        height: childrenRect.height
        
        StyledText {
            id: artist
            width: parent.width
            text: qsTr("Device: ")  + dcenowplaying.qs_mainTitle       
            wrapMode: "NoWrap"
            font.bold: true
            state:"header"
            smooth: true        
            color: "orange"
        }
        
        StyledText {
            id: network_id
            wrapMode: "NoWrap"
            text: qsTr("Network: ") + dcenowplaying.channelID
            state:"para"
            font.bold: true
            smooth: true
            horizontalAlignment: Text.AlignHCenter          
            color: "white"
        }
        
        
        StyledText {
            id: channel_id
            wrapMode: "NoWrap"
            text: qsTr("Channel: ") + dcenowplaying.channel           
            font.bold: true
            smooth: true
            horizontalAlignment: Text.AlignHCenter        
            color: "white"
        }
        
        StyledText {
            id: program_title
            wrapMode: "NoWrap"
            text: qsTr("Program:") + dcenowplaying.tvProgram           
            font.bold: true
            smooth: true
            horizontalAlignment: Text.AlignHCenter           
            color: "white"
        }
    }
}
