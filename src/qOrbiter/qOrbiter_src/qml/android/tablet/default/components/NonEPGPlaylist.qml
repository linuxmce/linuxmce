import QtQuick 1.0
import "../../../../skins-common/lib/handlers"

Item {
    id:nonepgplaylist
    width: scaleX(25)
    height: scaleY(75)
    property bool optionVisible: false
    property alias playlistDelegate:nonepgplaylistview.delegate

    Rectangle{
        id:phil
        anchors.fill: parent
        color:"blue"
        opacity: .15
        border.width: 2
        border.color: "white"
    }
    Connections{
        target: mediaplaylist
        onActiveItemChanged: indexUpdate.start()
    }

    Timer{
        id:indexUpdate
        interval: mediaplaylist.count()*100
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
        width: parent.width
        height: parent.height
        anchors.centerIn: parent
        clip: true
        interactive: true
        flickableDirection: "VerticalFlick"
        model: mediaplaylist
        onContentYChanged:console.log(contentY)
        delegate:DefaultPlaylistDelegate{
            moving:nonepgplaylistview.movingVertically

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
