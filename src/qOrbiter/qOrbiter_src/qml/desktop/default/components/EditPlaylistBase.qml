import QtQuick 1.1

Rectangle {
    id:editPlaylistBase
    anchors.fill: parent
    color: "transparent"

    Rectangle{
        id:mask
        anchors.fill: parent
        color: "black"
        opacity: .25
    }

    Rectangle{
        height: 100
        width: 100
        color: "grey"
        StyledText{
            anchors.centerIn: parent
            text: "Close"
        }
        MouseArea{
            anchors.fill: parent
            onClicked: editPlaylistBase.destroy();
        }
    }

    Rectangle{
        id:editorContainer
        height: scaleY(45)
        width: scaleX(65)
    }
}
