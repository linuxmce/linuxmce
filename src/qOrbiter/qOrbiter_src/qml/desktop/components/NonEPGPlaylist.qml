import QtQuick 1.0

Rectangle {
    width: scaleX(20)
    height: scaleY(80)
    color: "transparent"

    Component.onCompleted: nonepgplaylistview.positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Beginning)

    Connections{
        target: dcenowplaying
        onPlayListPositionChanged:{
            nonepgplaylistview.positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Beginning)

        }

    }


    ListView{
        id:nonepgplaylistview
        width: scaleX(20)
        height: scaleY(80)

        highlightFollowsCurrentItem: true
        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        clip: true
        interactive: true
        flickableDirection: "VerticalFlick"
        model: mediaplaylist

        delegate:
            Rectangle {
            border.color: "black"
            border.width: 1
            width:scaleX(20)
            height: scaleY(10)
            color: style.accentcolor
            clip: true
            Image {
                id: playlistimage
                fillMode: Image.PreserveAspectCrop
                source:  index == dcenowplaying.m_iplaylistPosition ? playlistimage.source = "image://updateobject/"+securityvideo.timestamp: ""
                anchors.fill: parent
                opacity: .4
            }
            Text {
                id: position
                text: name
                font.family: "DroidSans"
                opacity: .5
                color: "black"
                anchors.centerIn: parent
                font.pixelSize: 18
                font.bold: true
                font.capitalization: Font.SmallCaps
            }

            Text {
                text: id
                font.family: "DroidSans"
                color: "black"
                width: parent.width
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                font.pixelSize: 18
                font.bold: true
                font.capitalization: Font.SmallCaps
            }

            MouseArea{
                anchors.fill: parent
                onClicked: changedPlaylistPosition(name)
            }
        }
    }
}
