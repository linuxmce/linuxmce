import QtQuick 1.1

Item {
    height: parent.height
    width: parent.width
    onActiveFocusChanged: {

        if(activeFocus){
            fd_close.forceActiveFocus();
            console.log("File Details got focus, passed to controls")
        }
    }

    Rectangle{
        id:fd_hdr
        height:scaleY(15)
        width: scaleX(75)
        color:skinStyle.mainColor
        opacity: .65
    }
    StyledText {
        text: manager.q_mediaType
        fontSize: headerText
        font.bold: true
        anchors.right: fd_hdr.right
    }

    StyledText {
        id: fd_hdr_text
        text: qsTr("")
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
        property int childCount: 3

        function moveFocus(idx){
            console.log("moving focus for == >" + fd_action_row.children[idx].actionLabel)
            if(idx !==-1 && idx !==childCount ){
                for (var i = 0; i < fd_action_row.children.length; i++){
                    if(fd_action_row.children[i].rowIndex===(idx)){
                        fd_action_row.children[i].forceActiveFocus()
                        console.log("Focus set to true for "+ fd_action_row.children[i].actionLabel)
                    }
                }
            }
        }

        FdActionButton {
            id: fd_play
            actionLabel: "Play"
            rowIndex: 0
            onExecute:  {manager.playMedia(filedetailsclass.file); screen_forty_seven_of_nine.state="browsing"; dataModel.setPause(false)}  //dce function
        }
        FdActionButton {
            id: fd_move
            actionLabel: "Move"
            rowIndex: 1
        }
        FdActionButton {
            id: fd_close
            actionLabel: "Close"
            rowIndex: 2
            onExecute: { filedetailsclass.clear();screen_forty_seven_of_nine.state="browsing"; dataModel.setPause(false)  }
        }

    }


    states: [
        State {
            name: "stored_audio"
            when: manager.q_mediaType === "4"
            PropertyChanges {
                target: fd_hdr_text
                text:qsTr("Audio File")+filedetailsclass.file
            }
        },
        State {
            name: "stored_video"
            when: manager.q_mediaType === "5"
            PropertyChanges {
                target: fd_hdr_text
                text: qsTr("Video")+filedetailsclass.file
            }
        },
        State {
            name: "stored_media_playlist"
            when: manager.q_mediaType === "21"
            PropertyChanges {
                target: fd_hdr_text
                text:qsTr("Playlist")+filedetailsclass.file
            }
        },
        State {
            name: "unknown"

            PropertyChanges {
                target: fd_hdr_text
                text:qsTr("Unknown Media Type ==>") + manager.q_mediaType
            }
        }
    ]

}
