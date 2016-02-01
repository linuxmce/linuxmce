import QtQuick 1.1
import Qt.labs.shaders 1.0
import AudioVisual 1.0


MediaManager{
    id:dceplayer
    anchors.top: parent.top
    anchors.left:parent.left
    
    onFocusChanged: console.log("DCEPlayer Internal focus::"+focus)
    onActiveFocusChanged: {
        if(activeFocus){
            console.log("Media Player has focus")
            pageLoader.forceActiveFocus()
        }
    }

    ListView{
        id:audioOutputs
        height: manager.appHeight / 2
        width: manager.appWidth
        model:dceplayer.outputs
        delegate: Text{
            text:"this is an output"+model.data
        }
    }
    
    Component.onCompleted: {
        setorbiterWindowSize(manager.appHeight, manager.appWidth);
    }
    
    Connections{
        target:manager
        onOrientationChanged:dceplayer.setorbiterWindowSize(manager.appHeight, manager.appWidth)
        onMediaPlayerIdChanged:{
            console.log("initializing media player"+manager.mediaPlayerID)
            dceplayer.setConnectionDetails(manager.mediaPlayerID, manager.currentRouter)
        }
    }
    
    onCurrentStatusChanged:logger.logMediaMessage("Media Player Status::"+dceplayer.currentStatus)
    onMediaBufferChanged: console.log("media buffer change:"+mediaBuffer)
    onMediaPlayingChanged: console.log("Media Playback status changed locally "+dceplayer.mediaBuffer)
    onVolumeChanged:console.log(volume)
    Keys.onVolumeDownPressed: manager.adjustVolume("-1")
    Keys.onVolumeUpPressed:  manager.adjustVolume("+1")
    Keys.onTabPressed: ftr.forceActiveFocus()
    
    Keys.onPressed: {
        
        switch(event.key){
        case Qt.Key_Back:
            manager.changedPlaylistPosition((dcenowplaying.m_iplaylistPosition-1));
            break;
        case Qt.Key_Forward:
            manager.changedPlaylistPosition((dcenowplaying.m_iplaylistPosition+1))
            break;
        case 16777347: /* Keycode Track forward */
            manager.changedPlaylistPosition((dcenowplaying.m_iplaylistPosition+1));
            break;
        case 16777346: /* Keycode Track Backwards */
            manager.changedPlaylistPosition((dcenowplaying.m_iplaylistPosition-1))
            break;
        case Qt.Key_Plus: /*Plus sign */
            manager.adjustVolume(+1)
            break;
            
        case Qt.Key_VolumeMute:
            manager.mute()
            break;
            
        case Qt.Key_M:
            manager.mute()
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
}
