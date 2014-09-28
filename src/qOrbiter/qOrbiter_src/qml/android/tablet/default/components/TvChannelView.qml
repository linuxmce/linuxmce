import QtQuick 1.1

Item {
    anchors{
        top:parent.top
        left:parent.left
        right:parent.right
        bottom:parent.bottom
    }

//    Component.onCompleted: {
//      //  manager.getLiveTVPlaylist();
//        manager.setNowPlayingTv()
//    }
    clip:true

    Connections{
        target: simpleepg
        onProgramChanged:epgplaylistview.positionViewAtIndex(simpleepg.currentIndex, ListView.Beginning)
    }
    Rectangle{
        anchors.fill: parent
        color: "black"
        opacity:.65
    }

    ListView{
        id:epgplaylistview
        height:scaleY(85)
        width: scaleX(85)
        anchors.centerIn: parent
        clip:false
        interactive: true
        flickableDirection: "VerticalFlick"
        model: simpleepg
        delegate: Item {
            id:delrect
            height: scaleY(20)
            width:parent.width

            Rectangle{
                anchors.fill: parent
                color:"green"
                opacity: .85
                border.color: "white"
                border.width: 1
            }

            StyledText {
                id:proglabel
                text:program
                width: parent.width *.75
                fontSize: 28
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                color: "white"
            }

            Rectangle{
                id:channelidrect
                height: parent.height - 1
                width: parent.width*.25
                color: style.maincolor
                anchors.right: parent.right
                border.color: style.darkhighlight
                border.width: 2

                StyledText {
                    id:channelID
                    text: name
                    wrapMode: "WrapAnywhere"
                    font.pixelSize: 12
                    anchors.top: parent.top
                    font.bold: true
                    fontSize: 22
                    color: "whitesmoke"
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                StyledText{
                    text: channelnumber
                    fontSize: 32
                }

                MouseArea{
                    anchors.fill: parent
                    onClicked:{
                        manager.changeChannels(channelNumber)
                       // manager.gridChangeChannel(channelnumber, channelid)
                        epgplaylistview.positionViewAtIndex(index, ListView.Beginning)
                    }
                }
            }
        }
    }
}
