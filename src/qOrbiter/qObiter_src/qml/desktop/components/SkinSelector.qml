import QtQuick 1.0


Item {
    id: skinchooser
    anchors.centerIn: parent
    height: 350
    width: 350

Rectangle {
    width: skinchooser.width-5
    height: skinchooser.height -5
    color: "lightsteelblue"
    Text {
        id: labelforview
        text: "Please Choose a skin"
    }

    ListView
    {
        id:skinlist
        height: parent.height
        width: parent.width
        delegate: Rectangle{
            Text {
                id: skinname
                text: "text"
            }
            Text {
                id: skindescription
                text: "text"
            }
        }
    }
}

}
