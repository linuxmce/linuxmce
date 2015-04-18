import QtQuick 2.2
import org.linuxmce.enums 1.0
import org.linuxmce.screens 1.0
Item {
   id:generic_playback
   property bool showingPlaylist:false

   Item{
       id:directionDiamond
   }

   Item{
       id:arrows
   }

   Item{
       id:lightingAudioControls
   }

   Item{
       id:mediaScrollerTarget
   }

   Item{
       id:playlistPanel
   }

   states: [
       State {
           name: "fallback"
           extend: ""
       },
       State {
           name: "localvideo"
           extend:""
           PropertyChanges {
               target: generic_playback
               showingPlaylist: playlistPanel.itemCount > 1 ? true : false

           }
       },
       State {
           name: "localaudio"

       },
       State{
           name:"dvd"
       },
       State{
           name:"streamingaudio"
       },
       State{
           name:"radio"
       },
       State{
           name:"airplay"
       },
       State{
           name:"networkmedia"
       },
       State{
           name:"options"
       }
   ]

}
