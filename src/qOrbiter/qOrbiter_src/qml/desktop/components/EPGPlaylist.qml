import QtQuick 1.0

Rectangle {
    width: scaleX(26)
    height: scaleY(61)
    color: style.lighthighlight

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
        height:scaleY(60)
        width: scaleX(25)     
        anchors.centerIn: parent
        clip: true
        interactive: true
        flickableDirection: "VerticalFlick"
        model: simpleepg

        delegate:

            Rectangle {
            id:delrect
            border.color: "black"
            border.width: 2
            width:scaleX(24)
            height: scaleY(10)
            color: style.lighthighlight

            Rectangle{
                height: parent.height - 1
                width: scaleX(3)
                color: style.darkhighlight
                anchors.right: parent.right
                border.color: style.accentcolor
                border.width: 1
                Text {
                    id: position
                    text: channelnumber
                    font.bold: true
                    font.family: "DroidSans"

                    opacity: .5
                    color: "black"
                    anchors.centerIn: parent
                    font.pixelSize: 18

                }
            }


            Text {
                id:channelID
                text: name
                font.capitalization: Font.SmallCaps
                anchors.fill: parent
                wrapMode: "WrapAnywhere"
                font.pixelSize: 12
                anchors.top: parent.top

                font.bold: true

            }

            Text {
                text:program
                anchors.bottom: delrect.bottom
                anchors.bottomMargin: 5
                font.capitalization: Font.SmallCaps

                wrapMode: "WrapAnywhere"
                font.pixelSize: 16
                font.bold: true


            }

            MouseArea{
                anchors.fill: parent
                onClicked:{
                    gridChangeChannel(channelnumber, channelid)
                    nonepgplaylistview.positionViewAtIndex(index, ListView.Beginning)
                    dcenowplaying.setProgram(program)
                }
                hoverEnabled: true
                onEntered: delrect.color = style.accentcolor
                onExited: delrect.color = style.lighthighlight
            }
        }
    }
}
