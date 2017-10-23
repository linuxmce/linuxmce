import QtQuick 1.1
import "../../../skins-common/lib/handlers"

Rectangle {
    id:nonepgplaylist


    visible: nonepgplaylistview.count > 1 ? true : false
    color: "transparent"
    clip:true
    Rectangle{
        anchors.fill: parent
        color: "black"
        opacity: .35
    }

    property bool optionVisible: false

    Component{
        id:del
        Rectangle {
               width:scaleX(25)
               height:index === dcenowplaying.m_iplaylistPosition ? scaleY(12) : scaleY(8)
               anchors.horizontalCenter: parent.horizontalCenter
               color: "transparent"
               clip: true
               Image {
                   id: playlistimage
                   fillMode: Image.PreserveAspectCrop
                   source: index === dcenowplaying.m_iplaylistPosition ? playlistimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp: manager.imagePath+"ui3/grey_cell.png"
                   anchors.fill: parent
                   opacity: .5
               }
               Text {
                   id: position
                   text: qsTr("Item #") + value + " of " + nonepgplaylistview.count
                   font.family: "DroidSans"
                   color: "aliceblue"
                   font.pixelSize: scaleY(2.25)
                   font.bold: true
                   anchors.bottom: parent.bottom
                   opacity: .75
               }

               Text {
                  // text:  index === dcenowplaying.m_iplaylistPosition ? "Now Playing - " + name : name
                   text:description
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



    Behavior on opacity{
        PropertyAnimation{
            duration: 1000
        }
    }
    Connections{
          target: dcenowplaying
          onPlayListPositionChanged:{
             nonepgplaylistview.positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Beginning)
          }
      }
    Connections{
        target:manager
        onPlaylistSizeChanged:{
            console.log("playlist size change")
          //  manager.refreshDataGrid("Playlist", 18, "38")
            //nonepgplaylistview.model = manager.getDataGridModel("Playlist", 18)
        }
    }

    ListView{
        id:nonepgplaylistview
        width: scaleX(25)
        height:parent.height
        anchors.top: parent.top
        clip: true
        interactive: true
        flickableDirection: "VerticalFlick"
        model:manager.getDataGridModel("Playlist", 18)
        delegate:del

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
                height: skinstyle.avoptionbuttonh
                width: skinstyle.avoptionbuttonw
                color: skinstyle.darkhighlight
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
                height: skinstyle.avoptionbuttonh
                width: skinstyle.avoptionbuttonw
                color: skinstyle.darkhighlight
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
                height: skinstyle.avoptionbuttonh
                width: skinstyle.avoptionbuttonw
                color: skinstyle.darkhighlight
                Text {
                    id: private_text
                    text: qsTr("Private")
                    anchors.centerIn: parent
                }
            }
        }
    }

    states: [
        State {
            name: "hidden"
            PropertyChanges {
                target: nonepgplaylist
                height:0
                width:0
                opacity:0
            }
        },
        State {
            name: "exposed"
            PropertyChanges {
                target: nonepgplaylist
                width: scaleX(25)
                height: scaleY(45)
                opacity:1
            }
        }
    ]
}
