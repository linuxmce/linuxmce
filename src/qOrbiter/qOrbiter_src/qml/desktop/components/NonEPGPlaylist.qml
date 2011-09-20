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
            color: style.darkhighlight
            clip: true
            Image {
                id: playlistimage
                fillMode: Image.PreserveAspectCrop
                source:  index == dcenowplaying.m_iplaylistPosition ? playlistimage.source = "image://updateobject/"+securityvideo.timestamp: ""
                anchors.fill: parent
                opacity: .5
            }
            Text {
                id: position
                text: qsTr("Item #") + name
                font.family: "DroidSans"

                color: "aliceblue"

                font.pixelSize: 18
                font.bold: true
                font.capitalization: Font.SmallCaps
                anchors.bottom: parent.bottom
                opacity: .75
            }

            Text {
                text: id
                font.family: "DroidSans"
                color: "aliceblue"
                width: parent.width
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                font.pixelSize: 18
                font.bold: true
                font.capitalization: Font.SmallCaps
            }

            Image {
                id: overlay
                fillMode: Image.PreserveAspectCrop
                source: "../../../img/icons/header.png"
                anchors.fill: parent
                opacity: .30
            }

            MouseArea{
                anchors.fill: parent
                onClicked: changedPlaylistPosition(name)
            }
        }
    }
}
