import QtQuick 1.0
import "../components"

Rectangle {

   // property alias synText:
    id: storedAudioRemote


    height: 480
    width: 320

    NowPlayingBox{ x: 74;y: 11}
    Rectangle{
                   id:textrect
                   visible: true
                   height: childrenRect.height
                   width: scaleX(40)
                   color:"transparent"
                   anchors.left:  imageholder.right;
                   anchors.top:  imageholder.top
                   anchors.bottomMargin: dcenowplaying.aspect == "wide"? 10 : 20
                   clip:true
                   Column{
                       spacing: scaleY(.5)
                       width: childrenRect.width
                       height: dcenowplaying.aspect == "wide"?scaleY(25): scaleY(35)
                       Text {
                           id: starring
                           width: scaleX(40)
                           wrapMode: "WrapAtWordBoundaryOrAnywhere"
                           text: dcenowplaying.performerlist
                           font.family: "Droid Sans"
                           font.bold: true
                           smooth: true
                           font.pixelSize: scaleY(4)
                           elide: "ElideRight"
                           visible:  dcenowplaying.performerlist =="" ? false: true

                           MouseArea{
                               anchors.fill: starring
                               hoverEnabled: true
                               onEntered: { starring.elide = "ElideNone" ; }
                               onExited: {starring.elide = "ElideRight"; }
                           }
                       }
                       Text {
                           id: generaltitle
                           width: scaleX(35)
                           text:  dcenowplaying.mediatitle
                           font.family: "Droid Sans"
                           wrapMode: "WrapAtWordBoundaryOrAnywhere"
                           smooth: true
                           font.pixelSize: scaleY(3.5)
                           visible:  dcenowplaying.mediatitle =="" ? false: true
                       }

                       Text {
                           id: programtext
                           width: scaleX(35)
                           text: qsTr("Album: ") + dcenowplaying.album
                           font.family: "Droid Sans"
                           wrapMode: "WrapAtWordBoundaryOrAnywhere"
                           smooth: true
                           font.pixelSize: scaleY(2)
                           visible:  dcenowplaying.album =="" ? false: true
                       }

                       Text {
                           id: episode
                           width: scaleX(35)
                           wrapMode: "WrapAtWordBoundaryOrAnywhere"
                           text: qsTr("Track: ") + dcenowplaying.track
                           font.family: "Droid Sans"
                           //font.bold: true
                           font.italic: true
                           smooth: true
                           font.pixelSize: scaleY(2)
                           visible:  dcenowplaying.track =="" ? false: true
                       }

                       Text {
                           id: genre
                           width: scaleX(35)
                           wrapMode: "WrapAtWordBoundaryOrAnywhere"
                           text: qsTr("Genre(s): ") + dcenowplaying.genre
                           font.family: "Droid Sans"
                           //font.bold: true
                           font.italic: true
                           smooth: true
                           font.pixelSize: scaleY(2)
                           visible:  dcenowplaying.genre =="" ? false: true
                           MouseArea{
                               anchors.fill: genre
                               hoverEnabled: true
                               onEntered: { genre.elide = "ElideNone" ; }
                               onExited: {genre.elide = "ElideRight"; }
                           }
                       }
                       Text {
                           id: released
                           width: scaleX(35)
                           wrapMode: "WrapAtWordBoundaryOrAnywhere"
                           text: qsTr("Released: ") + dcenowplaying.releasedate
                           font.family: "Droid Sans"
                           // font.bold: true
                           font.italic: true
                           smooth: true
                           font.pixelSize: scaleY(2)
                           visible:  dcenowplaying.releasedate =="" ? false: true

                       }

                   }
               }



    AudioRemote{ x: 25; y: 393; anchors.verticalCenterOffset: 193; anchors.horizontalCenterOffset: 5;anchors.centerIn: parent}
    color: "transparent"

    //main 'now playing rect containing all the other items
   // Remote_lighting_controls{ id: remote_lighting_controls1; x: 344; y: 351; width: 93; height: 219; anchors.topMargin: 179;anchors.top: nowplayingbox1.baseline}
   // Remote_Audio_controls{ id: remote1; x: 200; y: 351; anchors.rightMargin: 71; z: 45; anchors.right: remote_lighting_controls1.left}
 HaControls{ x: 0; y: 5;}
    HomeButton{ x: 275; y: 160; anchors.leftMargin: 275; anchors.topMargin: 160;anchors.left: storedAudioRemote.left; anchors.top:parent.top}


}



