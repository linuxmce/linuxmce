import QtQuick 2.0
import "../components"
StyledOSDScreen {
   screen: qsTr("Now Playing Audio Remote, OSD")
   onActiveFocusChanged: {
       if(activeFocus)
           audio_playback_remote.forceActiveFocus()
   }

   GenericOsdRemote{
       focus:true
       id:audio_playback_remote
   }
}

