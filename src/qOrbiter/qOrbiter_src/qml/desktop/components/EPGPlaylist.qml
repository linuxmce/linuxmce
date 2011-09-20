import QtQuick 1.0

Rectangle {
    width: scaleX(26)
    height: scaleY(61)
    color: style.lighthighlight
    Component.onCompleted: getLiveTVPlaylist()
    clip:true

    Connections{
        target: dcenowplaying
        onPlayListPositionChanged:epgplaylistview.positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Beginning)

    }

    Rectangle{
        height: childrenRect.height
        width: childrenRect.width
        anchors.centerIn: parent
        color: "aliceblue"

        ListView{
            id:epgplaylistview
            height:scaleY(60)
            width: scaleX(25)
            anchors.centerIn: parent
            clip:false
            interactive: true
            flickableDirection: "VerticalFlick"
            model: simpleepg

            delegate:

                Rectangle {
                id:delrect
                border.color: "aliceblue"
                border.width: 2
                width:scaleX(25)
                height: scaleY(10)
                color: style.lighthighlight

                Rectangle{
                    height: parent.height - 1
                    width: parent.width*.25
                    color: style.maincolor
                    anchors.right: parent.right
                    border.color: style.accentcolor
                    border.width: 2

                    Text {
                        id: position
                        text: channelnumber
                        color: "aliceblue"
                        font.bold: true
                        font.family: "DroidSans"
                        opacity: .5
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
                    width: parent.width *.75
                    elide: "ElideRight"
                    wrapMode: "WrapAnywhere"
                    font.pixelSize: 16
                    font.bold: true


                }

                MouseArea{
                    anchors.fill: parent
                    onClicked:{
                        gridChangeChannel(channelnumber, channelid)
                        epgplaylistview.positionViewAtIndex(index, ListView.Beginning)
                        dcenowplaying.setProgram(program)
                    }
                    hoverEnabled: true
                    onEntered: delrect.color = style.highlight2
                    onExited: delrect.color = style.lighthighlight
                }
            }
        }

        Image {
            id: headeroverlay
            source: "../../../img/icons/listoverlay.png"
            height: epgplaylistview.height
            width: epgplaylistview.width
            anchors.centerIn: epgplaylistview
            opacity: .35
        }
    }
}
