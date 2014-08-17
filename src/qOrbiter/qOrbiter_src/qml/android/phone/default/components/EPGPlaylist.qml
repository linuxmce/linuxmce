import QtQuick 1.0

Rectangle {
    id:droidepg_small

    color: "transparent"
    clip:true
   // Component.onCompleted: manager.setNowPlayingTv()
    Connections{
        target: simpleepg
        onProgramChanged:epgplaylistview.positionViewAtIndex(simpleepg.currentIndex, ListView.Beginning)
    }
    Rectangle{
        id:hdr
        height: scaleY(4)
        width: droidepg_small.width
        color: "#383737"
        anchors.top: droidepg_small.top
        anchors.left: droidepg_small.left
        Text {
            id: hdrtxt
            text: qsTr("Tv Channels")
            color:"orange"
            font.pixelSize: scaleY(2)
        }

    }

    Rectangle{
        height: childrenRect.height
        width: childrenRect.width
        anchors.top: hdr.bottom
        color: "transparent"
        clip:true

        ListView{
            id:epgplaylistview
            height: droidepg_small.height-hdr.height
            width: droidepg_small.width
            clip:true
            interactive: true
            flickableDirection: "VerticalFlick"
            model: simpleepg

            delegate:
                Rectangle {
                id:delrect
                border.color: "orange"
                border.width: 1
                width: droidepg_small.width
                height: scaleY(15)
                color: "transparent"

                Rectangle{
                    id:channelidrect
                    height: parent.height - 1
                    width: parent.width*.25
                    color:"transparent"
                    anchors.right: parent.right
                    border.color: "silver"
                    border.width: 1

                    Text {
                        id:channel_ID
                        text: name
                        font.capitalization: Font.SmallCaps
                        width: parent.width
                        wrapMode: "WrapAnywhere"
                        font.pixelSize:scaleY(2.5)
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
                        font.pixelSize: scaleY(4)
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
                    font.pixelSize:scaleY(2.5)
                    color: "aliceblue"
                }

                MouseArea{
                    anchors.fill: parent
                    onClicked:{
                        manager.newGridChannel(channelnumber, channelid)
                        epgplaylistview.positionViewAtIndex(index, ListView.Beginning)
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
