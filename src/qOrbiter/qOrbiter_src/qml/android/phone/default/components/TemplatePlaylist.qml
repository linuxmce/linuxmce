import QtQuick 1.1
import "../../../../skins-common/lib/handlers"
Item {
    id:nonepgplaylist
    width: manager.appWidth
    height: manager.appHeight
    clip:true

    property double spacetracker: ((phantom_spaceman.x+(manager.appWidth)) / manager.appWidth)
  //  Component.onCompleted: manager.setBoundStatus(true)
    property alias psPos:phantom_spaceman.x


    Connections{
        target: mediaplaylist
        onActiveItemChanged:{
            nonepgplaylistview.positionViewAtIndex(mediaplaylist.currentIndex  , ListView.Beginning)
             nonepgplaylistview.currentIndex = mediaplaylist.currentIndex
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
        width:parent.width - 20
        height: parent.height - 20
        clip: true
        //  opacity: parent.opacity
        anchors.centerIn: parent
        interactive: true
        flickableDirection: "VerticalFlick"
        model: mediaplaylist
        spacing:scaleX(2)
        delegate:
            Item{
            width:manager.appWidth-20
            height: scaleY(20)
            clip: true

            Rectangle{
                anchors.fill: parent
                color: "black"
                opacity: .85
                radius:5
                border.color: "white"
            }

            Image {
                id: playlistimage
                fillMode: Image.PreserveAspectCrop
                source:  index === dcenowplaying.m_iplaylistPosition ? playlistimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp: ""
                anchors.fill: parent
                opacity: source ==="" ? 0 : 1
            }

            Text {
                id: position
                text: qsTr("#") + dceindex
                color: "White"
                font.pixelSize: scaleY(4)
                anchors.bottom: parent.bottom
                opacity: .75
                anchors.right: parent.right
            }

            Text {
                text:  index === dcenowplaying.m_iplaylistPosition ? "Now Playing - " + name : name
                color: "white"
                width: parent.width
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                font.pixelSize: scaleY(5)
                font.bold: false
            }

            PlaylistClickedHandler{
                id:ps
                anchors.fill: parent
                drag.target: phantom_spaceman
                drag.axis: Drag.XAxis
                drag.filterChildren: true
                onReleased: {
                        phantom_spaceman.x=0
                }
            }
        }
}

        Item{
            id:phantom_spaceman
            height:manager.appHeight
            width: manager.appWidth
            onXChanged:{
                metadataContainer.x =( phantom_spaceman.x + metadataContainer.width)
                  if  (phantom_spaceman.x < manager.appWidth *(-.75) ) {
                    mediaPlaybackBase.state = "controls"
                }
            }
        }
    }
