import QtQuick 1.1
import "../../lib/handlers"
Item {
    id:template
    height: scaleY(100)
    width: scaleX(100)
    focus:true
    onActiveFocusChanged: {
        if(activeFocus){
            console.log("Template recieved active focus")
            console.log(dcenowplaying.qs_screen)
        }
    }

    TemplateListView {
        id: playlist
    }


    Rectangle{
        id:image_placeholder
        anchors.centerIn: parent
        height: scaleY(25)
        width: scaleX(25)
        color:"black"
        visible: false
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
        default:
            console.log(event.key)
            break
        }
    }

    states: [
        State {
            name: "video_playback"
            when:manager.i_current_mediaType === 5
            PropertyChanges {
                target: playlist
                state:"hidden"

            }
        }, State {
            when:manager.i_current_mediaType === 4
            name: "audio_playback"
            PropertyChanges {
                target: playlist
                state:"showing"
            }
        }
    ]
}
