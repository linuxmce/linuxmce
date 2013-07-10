import QtQuick 1.1

Item {
    height: parent.height
    width: parent.width
    onActiveFocusChanged: fd_close.forceActiveFocus()

    Rectangle{
        id:fd_hdr
        height:scaleY(15)
        width: scaleX(75)
        color:skinStyle.mainColor
        opacity: .65
    }

    StyledText {
        id: fd_hdr_text
        text: qsTr("Playlist")
        fontSize: headerText
        font.bold: true
        anchors.centerIn: fd_hdr
    }



    Row{
        id:fd_action_row
        width: scaleX(75)
        height: scaleY(20)
        anchors.bottom: parent.bottom
        focus:true
        property int currentItem: -1

        onCurrentItemChanged: {
            console.log(children.length)
        }

        FdActionButton {
            id: fd_play
            actionLabel: "Play"
        }
        FdActionButton {
            id: fd_move
            actionLabel: "Move"
        }
        FdActionButton {
            id: fd_close
            actionLabel: "Close"
        }

    }


    states: [
        State {
            name: "4"
            when: manager.i_current_mediaType === 4
            PropertyChanges {
                target: fd_hdr_text
                text:qsTr("Audio File")+filedetailsclass.file
            }
        },
        State {
            name: "5"
            when: manager.i_current_mediaType === 5
            PropertyChanges {
                target: fd_hdr_text
                text: qsTr("Video")+filedetailsclass.file
            }
        },
        State {
            name: "21"
            when: manager.i_current_mediaType === 21
            PropertyChanges {
                target: fd_hdr_text
                text:qsTr("Playlist")+filedetailsclass.file
            }
        }
    ]

}
