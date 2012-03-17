import QtQuick 1.0

Rectangle {
    id:nonepgplaylist
    width: scaleX(25)
    height: scaleY(55)
    color: "transparent"
    clip:false
    Connections{
        target: mediaplaylist
        onActiveItemChanged:{
            nonepgplaylistview.positionViewAtIndex(mediaplaylist.currentIndex, ListView.Beginning)
        }
    }
/*
    BorderImage {
        id: borderimg
        verticalTileMode: BorderImage.Round
        horizontalTileMode: BorderImage.Repeat
        source: "../img/icons/drpshadow.png"
        anchors.fill: nonepgplaylist
        anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
        border { left: 10; top: 10; right: 10; bottom: 10 }
        smooth: true
    }
*/
    ListView{
        id:nonepgplaylistview
        width: scaleX(25)
        height: scaleY(55)
        anchors.centerIn: parent       
        clip: true
        interactive: true
        flickableDirection: "VerticalFlick"
        model: mediaplaylist

        delegate:
            Rectangle {
            border.color: "black"
            border.width: 1
            width:scaleX(25)
            height: scaleY(12)
            anchors.horizontalCenter: parent.horizontalCenter
            color: "transparent"
            clip: true
            Image {
                id: playlistimage
                fillMode: Image.PreserveAspectCrop
                source:  index === dcenowplaying.m_iplaylistPosition ? playlistimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp: ""
                anchors.fill: parent
                opacity: .5
            }
            Text {
                id: position
                text: qsTr("Item #") + index
                font.family: "DroidSans"
                color: "aliceblue"
                font.pixelSize: scaleY(2.25)
                font.bold: true
                anchors.bottom: parent.bottom
                opacity: .75
            }

            Text {
                text:  index === dcenowplaying.m_iplaylistPosition ? "Now Playing - " + name : name
                font.family: "DroidSans"
                color: "aliceblue"
                width: parent.width
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                font.pixelSize: scaleY(2.15)
                //font.bold: true

            }

            Image {
                id: overlay
                fillMode: Image.PreserveAspectCrop
                source: "../img/icons/header.png"
                anchors.fill: parent
                opacity:index === dcenowplaying.m_iplaylistPosition ? .25 :  .15
            }

            MouseArea{
                anchors.fill: parent
                onClicked: dcerouter.jumpToPlaylistPosition(index)
            }
            Rectangle{
                id:remove_box
                height: parent.height *.25
                width: parent.height *.25
                color:"red"
                opacity: .5
                anchors.right: parent.right
                MouseArea{
                    anchors.fill: remove_box
                    onClicked: dcerouter.removePlaylistItem(index)
                }
            }
        }
    }
}
