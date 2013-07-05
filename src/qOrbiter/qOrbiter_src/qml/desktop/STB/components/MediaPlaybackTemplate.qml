import QtQuick 1.1

Item {
    height: scaleY(100)
    width: scaleX(100)
    focus:true
    onActiveFocusChanged: {
        if(activeFocus){
            console.log("Template recieved active focus")
            console.log(dcenowplaying.qs_screen)
        }
    }

    ListView{
        id:playlist
        height: scaleY(65)
        width: scaleX(25)
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        model: dcenowplaying.qs_screen==="Screen_63.qml" ? simpleepg : mediaplaylist

        delegate: Item{
            height: scaleY(15)
            width: parent.width
            Rectangle{
                anchors.fill: parent
                color: "black"
                opacity: .85
            }
            StyledText{
                id:label
                text:name
                fontSize: mediumText
                color:"white"
            }
        }
    }


    Rectangle{
        id:image_placeholder
        anchors.centerIn: parent
        height: scaleY(25)
        width: scaleX(25)
        color:"black"
        visible: manager.i_current_mediaType !== 11
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
