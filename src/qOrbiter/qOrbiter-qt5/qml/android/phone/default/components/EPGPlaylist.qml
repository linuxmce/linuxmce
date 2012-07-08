import QtQuick 1.0

Rectangle {
    id:droidepg_small
    width: scaleX(72.5)
    height: scaleY(85)
    color: "transparent"    
    clip:true

    Connections{
        target: simpleepg
        onProgramChanged:epgplaylistview.positionViewAtIndex(simpleepg.currentIndex, ListView.Beginning)
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
            height:scaleY(85)
            width: scaleX(72.5)
            clip:true
            interactive: true
            flickableDirection: "VerticalFlick"
            model: simpleepg

            delegate:
                Rectangle {
                id:delrect
                border.color: "orange"
                border.width: 1
                width: scaleX(72.5)
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
                        dcerouter.TuneToChannel(channelnumber, channelid)
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
