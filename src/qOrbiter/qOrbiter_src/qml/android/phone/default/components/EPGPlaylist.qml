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
        clip:true

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
                    id:channelidrect
                    height: parent.height - 1
                    width: parent.width*.25
                    color: style.maincolor
                    anchors.right: parent.right
                    border.color: style.darkhighlight
                    border.width: 2

                    Text {
                        id:channelID
                        text: name
                        font.capitalization: Font.SmallCaps
                        width: parent.width

                        wrapMode: "WrapAnywhere"
                        font.pixelSize: 12
                        anchors.top: parent.top
                        font.bold: true
                        color: "whitesmoke"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

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
                    id:proglabel
                    text:program
                    anchors.top: delrect.top
                    anchors.topMargin: 5
                    font.capitalization: Font.MixedCase
                    width: parent.width *.75

                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                     font.pixelSize: 1 * scaleY(2.5)
                    color: "aliceblue"
                }

                MouseArea{
                    anchors.fill: parent
                    onClicked:{
                        gridChangeChannel(channelnumber, channelid)
                        epgplaylistview.positionViewAtIndex(index, ListView.Beginning)
                        dcenowplaying.setProgram(program)
                    }
                    hoverEnabled: true
                    onEntered:{ delrect.color = style.accentcolor ; proglabel.color = "black"}

                    onExited: {delrect.color = style.lighthighlight; proglabel.color = "aliceblue"}
                }
            }
        }

        Image {
            id: headeroverlay
            source: "../images/listcover.png"
            height: epgplaylistview.height
            smooth: true
            width: epgplaylistview.width
            anchors.centerIn: epgplaylistview
            opacity: .15
        }
    }
}
