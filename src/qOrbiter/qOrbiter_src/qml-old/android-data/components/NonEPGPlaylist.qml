import QtQuick 1.0

Rectangle {
    id:nonepgplaylist
    width: scaleX(25)
    height: scaleY(55)
    color: "transparent"
    clip:false
    property bool optionVisible: false
    Connections{
          target: dcenowplaying
          onPlayListPositionChanged:{
              nonepgplaylistview.positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Beginning)
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
        model: manager.getDataGridModel("Playlist", 18)

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
                onClicked: manager.changedPlaylistPosition(index)
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
                    MouseArea{
                        anchors.fill: parent
                        onClicked: manager.movePlaylistEntry("", index)
                    }
                }

                Rectangle{
                    id:remove_box
                    height: scaleX(1)
                    width:scaleX(1)
                    color:"yellow"
                    MouseArea{
                        anchors.fill: remove_box
                        onClicked: manager.removePlaylistItem(index)
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
                    MouseArea{
                        anchors.fill: parent
                        onClicked: manager.movePlaylistEntry("+", index)
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
                MouseArea{
                    anchors.fill: parent
                    onClicked: manager.saveCurrentPlaylist(save_as.text, true)
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
