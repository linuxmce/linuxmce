import QtQuick 1.0

Rectangle {
    width: scaleX(25)
    height: scaleY(50)
    color: "transparent"

    Timer{
        id:singleshot
        repeat: false
        interval: 2000
        triggeredOnStart: false
        running: false

        onTriggered:getLiveTVPlaylist()
    }


    Component.onCompleted: nonepgplaylistview.positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Beginning)

    Connections{
        target: dcenowplaying
        onPlayListPositionChanged:nonepgplaylistview.positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Beginning)

    }

    WorkerScript{
        id:playlistpopulator
        source: getLiveTVPlaylist()

    }

    ListView{
        id:nonepgplaylistview
        height:scaleY(50)
        width: scaleX(21)

        highlightFollowsCurrentItem: true
        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        clip: true
        interactive: true
        flickableDirection: "VerticalFlick"
        model: simpleepg

        delegate:

            Rectangle {
            id:delrect
            border.color: "black"
            border.width: 1
            width:scaleX(20)
            height: scaleY(5)
            color: style.lighthighlight
            Text {
                id: position
                text: channelnumber
                font.family: "DroidSans"
                opacity: .5
                color: "black"
                anchors.centerIn: parent
                font.pointSize: 14
            }

            Text {
                text: program
                anchors.fill: parent
                wrapMode: "WrapAnywhere"
                font.pixelSize: 12
                anchors.centerIn: parent

            }

            MouseArea{
                anchors.fill: parent
                onClicked:{
                    gridChangeChannel(channelnumber, channelid)
                    nonepgplaylistview.positionViewAtIndex(index, ListView.Beginning)
                }
                hoverEnabled: true
                onEntered: delrect.color = style.accentcolor
                onExited: delrect.color = style.lighthighlight
            }
        }
    }
}
