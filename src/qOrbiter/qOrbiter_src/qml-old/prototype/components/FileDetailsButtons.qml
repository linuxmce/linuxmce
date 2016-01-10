import QtQuick 1.0

Row{
    id:controlrow
    anchors.bottom: parent.bottom
    anchors.bottomMargin: scaleY(1)
    anchors.horizontalCenter: parent.horizontalCenter
    spacing: scaleY(5)
    FileDetailsButton {
        id: buttonsq1
        radius: 10        
        //anchors.leftMargin: 18
        buttontext: "Play"
        MouseArea
        {
            anchors.fill: parent
            onClicked: {manager.playMedia(filedetailsclass.file) ; filedetailrect.destroy()} //dce function
        }
    }
    
    FileDetailsButton {
        id: buttonsq2

        radius: 10
        buttontext: "Move"
    }
    
    FileDetailsButton {
        id: buttonsq3

        radius: 10
        buttontext: "Close"
        x: ((parent.width/3)*2)
        MouseArea{
            anchors.fill:  parent
            onClicked: {
                filedetailsclass.clear()
                filedetailrect.destroy()
            }
        }
    }
}
