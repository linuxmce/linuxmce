import QtQuick 2.2
import org.linuxmce.enums 1.0
import org.linuxmce.screens 1.0
import QtGraphicalEffects 1.0
import "../."
ScrollRow{
    id:transport_buttons
    anchors{
        bottom:parent.bottom
        horizontalCenter: parent.horizontalCenter
    }
    width: parent.width
    contentHeight: height
    contentWidth: content.width
    height: appStyle.scaleY(8)
    
    Row{
        id:content
        height: parent.height
        width: childrenRect.width
        anchors.horizontalCenter: parent.horizontalCenter
        StyledButton{
            state:"remote"
            buttonText: qsTr("Stop")
            onActivated: manager.stopMedia()
        }
        StyledButton{
            state:"remote"
            buttonText: qsTr("Prev")
            onActivated: manager.newTrack("-1")

        }
        StyledButton{
            state:"remote"
            buttonText: qsTr("RW")
            onActivated: manager.rewindMedia()
        }
        StyledButton{
            state:"remote"
            buttonText: qsTr("Pause")
            onActivated: manager.pauseMedia()
        }
        StyledButton{
            state:"remote"
            buttonText: qsTr("Play")
            onActivated: manager.playResume()
        }
        StyledButton{
            state:"remote"
            buttonText: qsTr("FF")
            onActivated: manager.fastForwardMedia()
        }
        StyledButton{
            state:"remote"
            buttonText: qsTr("Next")
            onActivated: manager.newTrack("+1")
        }
        
    }
}
