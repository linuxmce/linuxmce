import QtQuick 2.2


Item {
    id: skinchooser  
    height: manager.appHeight
    width: manager.appWidth

    Rectangle{
        id:mask
        color: "grey"
        anchors.fill: parent
        opacity: .65
        MouseArea{
            anchors.fill: parent
            onClicked: skinchooser.destroy()
        }
    }




    ListView
    {
        clip: true
        id:skinsListView
        anchors.centerIn: parent
        height: manager.appHeight*.65
        width: manager.appWidth/2
        model:skinsList
        delegate: SkinListDelegate{}
        spacing: scaleY(2)

    }

    StyledText {
           id: labelforview
           text: "Please Choose a skin"
           anchors.top:parent.top
           anchors.horizontalCenter: parent.horizontalCenter
           color: "white"
       }
}
