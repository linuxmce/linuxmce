import QtQuick 1.0
import "../components"

Rectangle {

   // property alias synText:
    id: storedAudioRemote


    height: appH
    width: appW

    NowPlayingBox{id:np_box
        height:scaleY(35)
        width: scaleX(85)
         visible: true
         anchors.left: ha.right
    }




    AudioRemote{ id:controls; anchors.bottom: parent.bottom; anchors.horizontalCenter: parent.horizontalCenter}
    color: "transparent"

    //main 'now playing rect containing all the other items
   // Remote_lighting_controls{ id: remote_lighting_controls1; x: 344; y: 351; width: 93; height: 219; anchors.topMargin: 179;anchors.top: nowplayingbox1.baseline}
   // Remote_Audio_controls{ id: remote1; x: 200; y: 351; anchors.rightMargin: 71; z: 45; anchors.right: remote_lighting_controls1.left}
 HaControls{id:ha; anchors.left: parent.left ; anchors.top: parent.top

 }
  HomeButton{ id:home ;anchors.right: parent.right; anchors.top: parent.top}

  NonEPGPlaylist {
      id: nonepgplaylist3
      anchors.top: parent.top
      anchors.horizontalCenter: parent.horizontalCenter
      visible: false
      opacity: 0
  }

  ButtonSq {
      id: buttonsq1
      anchors.top: home.bottom
      anchors.right: home.right
      height: style.stdbuttonh
      width: style.stdbuttonw
      buttontext: qsTr("Playlist")
      MouseArea{
          anchors.fill: parent
          onClicked: storedAudioRemote.state = "State1"
      }
  }
  states: [
      State {
          name: "State1"



          PropertyChanges {
              target: textrect
              visible: false
          }

          PropertyChanges {
              target: np_box
              visible: false
          }

          PropertyChanges {
              target: nonepgplaylist3
              visible: true
              opacity: 1
          }
      }
  ]


}



