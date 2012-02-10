import QtQuick 1.0

Rectangle {
    id:droidepg_small
    width: 225
    height: 400
    color: "transparent"
    Component.onCompleted: getLiveTVPlaylist()
    clip:true

    Connections{
        target: dcenowplaying
        onPlayListPositionChanged:epgplaylistview.positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Beginning)
    }

    Image {
        id: bg
        source: "../img/bkg.png"
        anchors.fill: droidepg_small
    }

    Rectangle{
        height: childrenRect.height
        width: childrenRect.width
        anchors.centerIn: droidepg_small
        color: "transparent"
        clip:true

        ListView{
            id:epgplaylistview
            height:400
            width: 225
            anchors.centerIn: parent
            clip:true
            interactive: true
            flickableDirection: "VerticalFlick"
            model: simpleepg

            delegate:
                Rectangle {
                id:delrect
                border.color: "aliceblue"
                border.width: 1
                width:225
                height: 50
                color: "transparent"

                Rectangle{
                    id:channelidrect
                    height: parent.height - 1
                    width: parent.width*.25
                    color:"transparent"
                    anchors.right: parent.right
                    border.color: style.darkhighlight
                    border.width: 1

                    Text {
                        id:channelID
                        text: name
                        font.capitalization: Font.SmallCaps
                        width: parent.width

                        wrapMode: "WrapAnywhere"
                        font.pixelSize: 8
                        anchors.top: parent.top
                        color: "whitesmoke"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    Text {
                        id: position
                        text: channelnumber
                        color: "aliceblue"
                        font.bold: true
                        font.family: "DroidSans"
                        anchors.centerIn: parent
                        font.pixelSize: 12
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
                    font.pixelSize: 12
                    color: "aliceblue"
                }

                MouseArea{
                    anchors.fill: parent
                    onClicked:{
                        gridChangeChannel(channelnumber, channelid)
                        epgplaylistview.positionViewAtIndex(index, ListView.Beginning)
                        dcenowplaying.setProgram(program)
                        droidepg_small.destroy()
                    }

                }
            }
        }

        /*  Image {
            id: headeroverlay
            source: "../img/listcover.png"
            height: epgplaylistview.height
            smooth: true
            width: epgplaylistview.width
            anchors.centerIn: epgplaylistview
            opacity: .15
        } */
    }
}
