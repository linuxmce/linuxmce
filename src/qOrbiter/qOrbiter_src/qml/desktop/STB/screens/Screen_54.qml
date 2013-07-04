import QtQuick 1.1
import "../components"
Item {
    anchors.fill: parent
    Component.onCompleted: {
        forceActiveFocus()
    }

    StyledText{
        anchors.centerIn: parent
        text:"Screen 54 - audio remote"
        color:"white"
        fontSize:mediumText

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

        default:
            console.log(event.key)
            break
        }
    }
}
