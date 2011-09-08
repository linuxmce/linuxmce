import QtQuick 1.0

Rectangle {
    width: 200
    height: 500
    color: "transparent"

    Timer{
        id:singleshot
        repeat: false
        interval: 2000
        triggeredOnStart: false
        running: true

        onTriggered:getLiveTVPlaylist()
    }


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
        model: playlistmodel

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
                font.family: "DroidSans"
                opacity: .5
                color: "black"
                anchors.centerIn: parent
                font.pointSize: 14
            }

            Text {
                text: id
                anchors.fill: parent
                wrapMode: "WrapAnywhere"
            }

            MouseArea{
                anchors.fill: parent
                onClicked: gridChangeChannel(name)
            }
        }
    }
}
