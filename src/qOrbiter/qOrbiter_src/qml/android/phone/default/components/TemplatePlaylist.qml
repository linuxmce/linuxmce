import QtQuick 1.1
import "../../../../skins-common/lib/handlers"
Item {
    id:nonepgplaylist
   anchors.fill: parent
    clip:true

      Component.onCompleted: manager.setBoundStatus(true)

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
        width:parent.width - 20
        height: parent.height - 20
        clip: true
        //  opacity: parent.opacity
        anchors.centerIn: parent
        interactive: true
        flickableDirection: "VerticalFlick"
        model: manager.getDataGridModel("Playist", 18)
        spacing:scaleX(2)
        delegate:
            Item{
            width:manager.appWidth-20
            height: scaleY(12)
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


            Rectangle{
                anchors.fill: parent
                gradient:Gradient{
                             GradientStop{ position:0.0;color:"transparent"}
                             GradientStop{ position:0.65;color:index === dcenowplaying.m_iplaylistPosition ? "darkgreen": "black"}
                         }
            }
            StyledText {
                id: position
                text: qsTr("#") + value
                color: "White"
                font.pixelSize: scaleY(4)
                anchors.bottom: parent.bottom
                opacity: .75
                anchors.right: parent.right
            }

            StyledText {
                text:  index === dcenowplaying.m_iplaylistPosition ? "Now Playing:\n " + description : description
                color: "white"
                width: parent.width

                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                font.pixelSize: scaleY(3)
                font.bold: false
            }

            PlaylistClickedHandler{
                id:ps
                anchors.fill: parent
            }
        }
    }


}
