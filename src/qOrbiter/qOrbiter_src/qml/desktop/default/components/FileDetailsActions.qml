import QtQuick 1.0


Rectangle{
    id:controlrow
   width: parent.width
   color: "transparent"
   anchors.bottomMargin: scaleY(2)

   Rectangle{
       id:transparency
       anchors.fill: parent
       color: "black"
       opacity: .25
   }

    Row{
        width: parent.width
        height: childrenRect.height
        spacing: scaleX(.5)

    AvOptionButton {
        id: buttonsq1
        width: parent.width/3
        height: style.stdbuttonh
        radius: 10
        //anchors.leftMargin: 18
        buttontext: "Play"
        MouseArea
        {
            anchors.fill: parent
            onClicked: manager.playMedia(filedetailsclass.file)  //dce function
        }
    }

        AvOptionButton {
            id: buttonsq2
            width:  parent.width/3
            height: style.stdbuttonh
            radius: 10
            buttontext: "Move"
        }

        AvOptionButton {
            id: buttonsq3
            width:  parent.width/3
            height: style.stdbuttonh
            radius: 10
            buttontext: "Close"          
            MouseArea{
                anchors.fill:  parent
                onClicked: { filedetailsclass.clear();filedetailrect.destroy()}
            }
        }
    }


}
