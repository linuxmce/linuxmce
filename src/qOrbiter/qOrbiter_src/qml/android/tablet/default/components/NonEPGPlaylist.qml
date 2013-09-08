import QtQuick 1.0
import com.nokia.android 1.1
import "../../../skins-common/lib/handlers"

Rectangle {
    id:nonepgplaylist
    width: scaleX(25)
    height: scaleY(55)
    color: "transparent"
    clip:false
    property bool optionVisible: false
    Connections{
        target: mediaplaylist
        onActiveItemChanged: indexUpdate.start()
}
        Timer{
            id:indexUpdate
            interval: mediaplaylist.count()*10
            running: false
            onTriggered: {
                console.log("interval is" + interval)
                nonepgplaylistview.positionViewAtIndex(mediaplaylist.currentIndex, ListView.Beginning)
                console.log("Current index is " + mediaplaylist.currentIndex)
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
            ListItem {
            width:scaleX(25)
            height: scaleY(12)
            clip: true
            Image {
                id: playlistimage
                fillMode: Image.PreserveAspectCrop
                source:  index === dcenowplaying.m_iplaylistPosition ? playlistimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp: ""
                anchors.fill: parent
                opacity: .5

            }
            ListItemText {
                id: position
                text: qsTr("Item #") + index
                font.family: "DroidSans"
                color: "aliceblue"
                font.pixelSize: scaleY(2.25)
                font.bold: true
                anchors.bottom: parent.bottom
                opacity: .75
            }

            ListItemText {
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
