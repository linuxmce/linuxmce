import QtQuick 2.2
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
            manager.setGridStatus(false)
        }
    }
    MediaPlaybackHeader {
        id: npHeader
    }

    Item{
        id:mediaInformation
        height: imageholder.height *.95
        anchors{
            verticalCenter: parent.verticalCenter
            left: playlist.right
            right: parent.right
        }

        clip:false
        Rectangle{
            id:phil
            color: skinStyle.mainColor
            anchors.fill: parent
            clip:false
            opacity: playlist.state === "showing" ? .75 : 0
            Behavior on opacity {
                PropertyAnimation{
                    duration:350

                }
            }
        }

        Loader{
            id:textCol
            source:"Metadata"+manager.i_current_mediaType+".qml"
            visible:playlist.state==="showing" ? true : false
            anchors{
                top:parent.top
                bottom:parent.bottom
                left:parent.left
            }
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

    NowPlayingImage {
        id: imageholder
        anchors{
            verticalCenter:template.verticalCenter
            right:template.right
        }

        // anchors.topMargin: scaleY(20)
        // anchors.rightMargin: scaleX(15)
    }

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
