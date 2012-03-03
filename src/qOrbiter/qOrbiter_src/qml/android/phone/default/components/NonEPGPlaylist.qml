import QtQuick 1.0

Rectangle {
    id:nonepgplaylist
    width: scaleX(80)
    height: scaleY(75)
    color: "silver"
    border.color: "orange"
    border.width: 2
    clip:false    
    function setupMedia()
    {
        dcerouter.SetNowPlayingDetails();
        nonepgplaylistview.positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Beginning)
    }

    Component.onCompleted: setupMedia()

    Connections{
        target: dcenowplaying
        onPlayListPositionChanged:{
            nonepgplaylistview.positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Beginning)
        }
    }

  /*  BorderImage {
        id: borderimg
        verticalTileMode: BorderImage.Round
        horizontalTileMode: BorderImage.Repeat
        source: "../img/drpshadow.png"
        anchors.fill: nonepgplaylist
        anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
        border { left: 10; top: 10; right: 10; bottom: 10 }
        smooth: true
    }
    */

    ListView{
        id:nonepgplaylistview
        width: scaleX(80)
        height: scaleY(75)
        anchors.centerIn: parent
        highlightFollowsCurrentItem: true
        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        clip: true
        interactive: true
        flickableDirection: "VerticalFlick"
        model: mediaplaylist


        delegate:
            Rectangle {
            border.color: "silver"
            border.width: 1
            width:scaleX(80)
            height: scaleY(20)
            anchors.horizontalCenter: parent.horizontalCenter

            Image {
                id: bg
                source: "../img/lowerbkg.png"
                width:scaleX(80)
                height: scaleY(20)
                fillMode: Image.PreserveAspectCrop
            }
            clip: true
            Image {
                id: playlistimage
                fillMode: Image.PreserveAspectCrop
                source:  index === dcenowplaying.m_iplaylistPosition ? playlistimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp: ""
                anchors.fill: parent
                opacity: 1
            }
            Text {
                id: position
                text: qsTr("#") + name
                font.family: "DroidSans"
                color: "silver"
                font.pixelSize: scaleY(4)
                font.bold: true

                anchors.bottom: parent.bottom
                opacity: .75
            }

            Text {
                text:  index === dcenowplaying.m_iplaylistPosition ? "Now Playing - " + id : id
                font.family: "DroidSans"
                color: "silver"
                width: parent.width
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                font.pixelSize: scaleY(4)
            }

           Image {
                id: overlay
                fillMode: Image.PreserveAspectCrop
                source: "../img/transparencymask.png"
                anchors.fill: parent
                opacity: .30
            }

            MouseArea{
                anchors.fill: parent
                onClicked: changedPlaylistPosition(name)
                onPressed: border.color = "orange"
                onReleased: border.color="silver"
            }
        }
    }
}
