import QtQuick 2.2
Item{
    height: parent.height
    width: parent.width *.75

    StyledText {
        id: updating_time
      text: dceTimecode.qsCurrentTime  + " of " + dceTimecode.qsTotalTime
        fontSize:32
        color: "white"
        anchors.left: parent.left
      visible:dceTimecode.qsTotalTime !==0
    }


    Row{
        width: childrenRect.width
        height: parent.height
        spacing:scaleX(2)
        anchors.left: updating_time.right
        StyledButton{
            buttonText: "Advanced"
            hitArea.onReleased: qml_root.showAdvanced()

        }
        StyledButton{
            buttonText: "Playlist"
            hitArea.onReleased: qml_root.showPlaylist()

        }
        StyledButton{
            buttonText: "Metadata"
            hitArea.onReleased: qml_root.showMetadata()

        }
    }
}
