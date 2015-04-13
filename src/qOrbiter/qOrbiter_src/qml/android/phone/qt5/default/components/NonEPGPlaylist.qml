import QtQuick 2.2
import "../../../skins-common/lib/handlers"
Rectangle {
    id:nonepgplaylist
    width: scaleX(80)
    height: scaleY(75)
    color: "transparent"
    border.color: "orange"
    border.width: 1
    clip:false    



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

        highlightFollowsCurrentItem: true
        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        clip: true
        interactive: true
        flickableDirection: "VerticalFlick"
        model: manager.getDataGridModel("Playlist", 18)

        delegate:
            Rectangle {
            border.color: "silver"
            border.width: 1
            width:scaleX(80)
            height: scaleY(20)
            color:"transparent"

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
                text: qsTr("#") + dceindex
                font.family: "DroidSans"
                color: "silver"
                font.pixelSize: scaleY(4)
                font.bold: true
                anchors.bottom: parent.bottom
                opacity: .75
                anchors.right: parent.right
            }

            Text {
                text:  index === dcenowplaying.m_iplaylistPosition ? "Now Playing - " + name : name
                font.family: "DroidSans"
                color: "silver"
                width: parent.width
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                font.pixelSize: scaleY(2)
                font.bold: true
            }

           Image {
                id: overlay
                fillMode: Image.PreserveAspectCrop
                source: "../img/transparencymask.png"
                anchors.fill: parent
                opacity: .50
            }

           PlaylistClickedHandler{
               onPressed: border.color = "orange"
               onReleased: border.color="silver"
           }

        }
    }
}
