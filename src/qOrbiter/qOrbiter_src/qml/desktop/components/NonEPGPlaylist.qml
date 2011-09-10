import QtQuick 1.0

Rectangle {
    width: scaleX(20)
    height: scaleY(80)
    color: "transparent"

    Component.onCompleted: nonepgplaylistview.positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Beginning)

Connections{
target: dcenowplaying
onPlayListPositionChanged:nonepgplaylistview.positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Beginning)

}
WorkerScript{
id:playlistpopulator

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
            color: status ? "whitesmoke" : "green"
            Text {
                id: position
                text: name
                font.family: "DroidSans"
                opacity: .5
                color: "black"
                anchors.centerIn: parent
                font.pixelSize: 14
            }

            Text {
                text: id
                anchors.fill: parent
                wrapMode: "WrapAnywhere"
                font.pixelSize: 12
            }

            MouseArea{
                anchors.fill: parent
                onClicked: changedPlaylistPosition(name)
            }
        }
    }
}
