import QtQuick 1.0
import "../lib/handlers"

Rectangle {
    id:nonepgplaylist
    width: scaleX(25)
    height: scaleY(55)
    color: "transparent"
    clip:false
    property bool optionVisible: false

    Connections{
        target: mediaplaylist
        onActiveItemChanged:{
            nonepgplaylistview.positionViewAtIndex(mediaplaylist.currentIndex, ListView.Beginning)
        }
    }

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

            PlaylistClickedHandler {
            }

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
                    PlaylistMoveHandler {
                        direction: ""
                    }
                }

                Rectangle{
                    id:remove_box
                    height: scaleX(1)
                    width:scaleX(1)
                    color:"yellow"
                    PlaylistRemoveItemHandler {
                    }
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
                       direction: "+"
                   }
                }
            }
        }
    }

    Rectangle{
        id:option_box
        height: scaleY(10)
        width: parent.width
        visible: optionVisible
        anchors.top: nonepgplaylistview.bottom
        Row{
            height: parent.height
            width: parent.width
            spacing: scaleX(3)
            Rectangle{
                id:save_playlist
                height: style.avoptionbuttonh
                width: style.avoptionbuttonw
                color: style.darkhighlight
                TextInput {
                    id: save_as
                    text: qsTr("Edit name")
                    anchors.centerIn: parent
                    fillColor: "white"
                    onFocusChanged: save_as.text=""
                }

            }

            Rectangle{
                id:public_option
                height: style.avoptionbuttonh
                width: style.avoptionbuttonw
                color: style.darkhighlight
                Text {
                    id: public_text
                    text: qsTr("Public")
                    anchors.centerIn: parent
                }
                PlaylistSavePublicHandler {
                    playlistname: save_as.text
                }

            }
            Rectangle{
                id:private_option
                height: style.avoptionbuttonh
                width: style.avoptionbuttonw
                color: style.darkhighlight
                Text {
                    id: private_text
                    text: qsTr("Private")
                    anchors.centerIn: parent
                }
            }
        }
    }
}
