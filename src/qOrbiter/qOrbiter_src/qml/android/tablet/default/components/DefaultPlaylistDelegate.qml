import QtQuick 1.1
import "../../../../skins-common/lib/handlers"
Item {
    width:scaleX(25)
    height: scaleY(12)
    clip: true
    property  bool moving: false

    Image {
        id: playlistimage
        fillMode: Image.PreserveAspectCrop
        source:  index === dcenowplaying.m_iplaylistPosition ? playlistimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp: ""
        anchors.fill: parent
        opacity: .5

    }
    StyledText {
        id: position
        text: qsTr("Item #") + index
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        opacity:moving ? .75 :0
        color:"white"
        style: Text.Raised

        Behavior on opacity {
            PropertyAnimation{
                duration:350
            }
        }

    }

    StyledText {
        text:  index === dcenowplaying.m_iplaylistPosition ? "Now Playing - " + name : name
        width: parent.width
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        color:"white"
    }

    Image {
        id: overlay
        fillMode: Image.PreserveAspectCrop
        source: "../img/icons/header.png"
        anchors.fill: parent
        opacity:index === dcenowplaying.m_iplaylistPosition ? .25 :  .15
    }

    PlaylistClickedHandler{}


    Column{
        id:options
        visible: optionVisible
        height: scaleY(12)
        anchors.right: parent.right
        spacing: scaleY(2)

        Rectangle{
            id:move_up
            height: scaleX(1)
            width: scaleX(1)
            color:"green"
            Text {
                id: up
                text: qsTr("Up")
            }
            PlaylistMoveHandler{direction: "-"}
        }

        Rectangle{
            id:remove_box
            height: scaleX(1)
            width:scaleX(1)
            color:"yellow"
            PlaylistRemoveItemHandler{}
        }

        Rectangle{
            id:move_dwn
            height: scaleX(1)
            width: scaleX(1)
            color:"blue"
            Text {
                id: down
                text: qsTr("down")
            }
            PlaylistMoveHandler{
                direction:"+"
            }
        }
    }
}
