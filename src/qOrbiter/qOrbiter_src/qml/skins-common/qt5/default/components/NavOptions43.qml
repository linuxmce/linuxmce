import QtQuick 2.2
Item{
    height: parent.height
    width: parent.width *.75

    StyledText {
        id: updating_time
        text: dceTimecode.qsCurrentTime + " of " + dceTimecode.qsTotalTime
        fontSize:32
        color: "white"
        anchors.left: nowplayingimage.right
        anchors.bottom: nowplayingimage.bottom

    }

    Row{
        width: childrenRect.width
        height: parent.height
        spacing:scaleX(2)
        anchors.left: updating_time.right
        StyledButton{
            buttonText: "Advanced"
            hitArea.onReleased: pageLoader.item.state="controls"
             opacity:pageLoader.item.state==="controls" ? 0 : 1
        }
        StyledButton{
            buttonText: "Playlist"
            hitArea.onReleased: pageLoader.item.state="playlist"
             opacity:pageLoader.item.state==="playlist" ? 0 : 1
        }
        StyledButton{
            buttonText: "Metadata"
            hitArea.onReleased: pageLoader.item.state="metadata"
            opacity:pageLoader.item.state==="metadata" ? 0 : 1
        }
    }
}
