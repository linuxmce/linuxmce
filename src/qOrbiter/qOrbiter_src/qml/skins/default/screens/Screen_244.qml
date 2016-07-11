import QtQuick 2.0
import "../components"
StyledOSDScreen {
   screen: qsTr("Now Playing Audio Remote, OSD")
   GenericOsdRemote{
       id:audio_playback_remote
   }
}

