import QtQuick 2.0
import "../../../skins-common/lib/handlers"
Item {
    id:template
    anchors{
        top:parent.top
        left:parent.left
        right:parent.right
        bottom:parent.bottom
    }
    focus:true
    onActiveFocusChanged: {
        if(activeFocus){
            console.log("Template recieved active focus")
            console.log(dcenowplaying.qs_screen)
            playlist.forceActiveFocus()
        }
    }

    Item{
        id:mediaInformation
        height: imageholder.height *.95
        width:  manager.appWidth
        anchors.centerIn: template
        clip:false
        Rectangle{
            id:phil
            color: skinStyle.mainColor
            height: parent.height
            width: playlist.state === "showing" ? (appW) : 0
            x: playlist.state === "showing"? 0: imageholder.x
            clip:false
            opacity: playlist.state === "showing" ? .95 : 0

            Behavior on x {
                PropertyAnimation{
                    duration: 750
                }
            }

            Loader{
                id:textCol
                source:"Metadata"+manager.i_current_mediaType+".qml"
                visible:playlist.state === "showing"
            }

            Row{
                id:temporalData
                height: childrenRect.height
                width: childrenRect.width
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.rightMargin: scaleX(15)
                visible: playlist.state === "showing"
                spacing:scaleX(2)
                StyledText {
                    id: updating_time
                    text: dceTimecode.qsCurrentTime
                    fontSize:32
                    color:"white"
                }

                StyledText {
                    text: qsTr("Of")
                    fontSize:32
                    color:"white"
                    font.bold: true
                }

                StyledText {
                    id: totalTime
                    text: dceTimecode.qsTotalTime
                    fontSize:32
                    color:"white"
                }
            }
        }
    }

    NowPlayingImage {
        id: imageholder
        anchors.top: parent.top
        anchors.topMargin: scaleY(20)
        anchors.right: parent.right
        anchors.rightMargin: scaleX(15)
    }


    Keys.onVolumeDownPressed: manager.adjustVolume("-1")
    Keys.onVolumeUpPressed:  manager.adjustVolume("+1")


    Keys.onPressed: {

        switch(event.key){
        case Qt.Key_Back:
            manager.changedPlaylistPosition((mediaplaylist.currentIndex-1));
            break;
        case Qt.Key_Forward:
            manager.changedPlaylistPosition((mediaplaylist.currentIndex+1))
            break;
        case 16777347: /* Keycode Track forward */
            manager.changedPlaylistPosition((mediaplaylist.currentIndex+1));
            break;
        case 16777346: /* Keycode Track Backwards */
            manager.changedPlaylistPosition((mediaplaylist.currentIndex-1))
            break;
        case Qt.Key_Plus: /*Plus sign */
            manager.adjustVolume(+1)
            break;

        case Qt.Key_Minus: /* Minus Sign */
            manager.adjustVolume(-1)
            break;
        case Qt.Key_T:
            if(playlist.state==="showing")
                playlist.state="hidden"
            else
                playlist.state = "showing"
            break;

        case Qt.Key_S:
            manager.stopMedia()
            break;

        case Qt.Key_Pause:
            manager.pauseMedia()
            break;
        case Qt.Key_P:
            manager.pauseMedia()
            break;

        case Qt.Key_PageUp:
            manager.changedPlaylistPosition(mediaplaylist.currentIndex-1)
            break;

        case Qt.Key_PageDown:
            manager.changedPlaylistPosition(mediaplaylist.currentIndex+1)
            break;
        default:
            console.log(event.key)
            break
        }
    }

    TemplateListView {
        id: playlist
    }

    states: [
        State {
            name: "video_playback"
            when:manager.i_current_mediaType === 5
            PropertyChanges {
                target: playlist
                state:"hidden"
            }
            PropertyChanges {
                target: imageholder
                visible:true
            }

        }, State {
            when:manager.i_current_mediaType === 4
            name: "audio_playback"
            PropertyChanges {
                target: playlist
                state:"showing"
            }
            PropertyChanges {
                target: imageholder
                visible:true
            }

        }
    ]

    MouseArea{
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        onClicked: {
            console.log(ftr.isActive)
            if(ftr.isActive){
                pageLoader.forceActiveFocus()
                ftr.state="hidden"
            }
            else
            {
                ftr.forceActiveFocus()
            }
        }
    }
}
