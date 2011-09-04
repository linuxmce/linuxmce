import QtQuick 1.0

Rectangle {
    width: 200
    height: 500
    color: "transparent"

    Component.onCompleted: nonepgplaylistview.positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Beginning)

Connections{
target: dcenowplaying
onPlayListPositionChanged:nonepgplaylistview.positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Beginning)
}

    ListView{
        id:nonepgplaylistview
        height:500
        width: 200

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
            width:200
            height: 50
            color: status ? "whitesmoke" : "green"
            Text {
                id: position
                text: name
                opacity: .5
                color: "black"
                anchors.centerIn: parent
                font.pointSize: 14
            }

            Text {
                text: id
                anchors.fill: parent
            }

            MouseArea{
                anchors.fill: parent
                onClicked: changedPlaylistPosition(name)
            }
        }
    }
}
