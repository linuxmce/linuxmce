import QtQuick 2.0


Item {
    id: skinchooser
    anchors.centerIn: parent
    height: 350
    width: 350

Rectangle {
    width: skinchooser.width-5
    height: skinchooser.height -5
    color: "lightsteelblue"
    StyledText {
        id: labelforview
        text: "Please Choose a skin"
    }
    SkinListDelegate{id:skinchooserdelegate}

    ListView
    {   anchors.top: labelforview.bottom
        clip: true
        id:skinsListView
        height: parent.height
        width: parent.width
        model:skinsList
        delegate: skinchooserdelegate

    }
}

}
