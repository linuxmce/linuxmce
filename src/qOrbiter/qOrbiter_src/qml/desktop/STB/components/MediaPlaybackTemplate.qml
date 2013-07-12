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


    Item{
        id:mediaInformation
        height: imageholder.height *.95
        width:  manager.appWidth
        anchors.centerIn: template
        clip:false
        Rectangle{
            id:phil
            color: skinStyle.mainColor
            height: parent.height
            width: playlist.state === "hidden" ? (appW) : 0
            x: playlist.state === "hidden"? 0: imageholder.x
            clip:false
            opacity: .95

            Behavior on x {
                PropertyAnimation{
                    duration: 750
                    easing.type: Easing.InOutBounce
                    easing.amplitude: .5
                }
            }

            Column{
                id:textCol
                spacing: scaleY(.5)
                width: childrenRect.width
                height: dcenowplaying.aspect == "wide"?scaleY(25): scaleY(35)
                visible: playlist.state === "hidden"
                StyledText {
                    id: generaltitle
                    width: scaleX(45)
                    text:  dcenowplaying.mediatitle
                    font.bold: true
                    //  wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    elide: "ElideRight"
                    smooth: true
                    fontSize: 40
                    visible:  dcenowplaying.mediatitle =="" ? false: true
                    color:skinStyle.accentcolor
                }
                StyledText { /* showing up with performers! fix */
                    id: title_text
                    width: scaleX(45)
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: dcenowplaying.qs_mainTitle
                    font.bold: true
                    font.italic: true
                    smooth: true
                    fontSize: 32
                    visible:  dcenowplaying.qs_mainTitle =="" ? false: true
                    opacity: .65
                    style: Text.Sunken
                }
                StyledText {
                    id: starring_text
                    width: scaleX(40)
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: dcenowplaying.performerlist == "" ? dcenowplaying.performerlist : qsTr("No Performer Information")
                    color: "Aliceblue"
                    smooth: true
                    fontSize: 32
                    elide: "ElideRight"
                    visible:false
                }

                StyledText {
                    id: album_text
                    width: scaleX(35)
                    text: dcenowplaying.album
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    smooth: true
                    font.bold: true
                    fontSize: 32
                    color:style.maincolor
                }

                StyledText {
                    id: genre_text
                    width: scaleX(35)
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: qsTr("Genre(s): ") + dcenowplaying.genre
                    smooth: true
                    fontSize: 32
                    font.bold: true
                    visible:  dcenowplaying.genre =="" ? false: true

                }
                StyledText {
                    id: released_text
                    width: scaleX(35)
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: qsTr("Released: ") + dcenowplaying.releasedate
                    // font.italic: true
                    smooth: true
                    font.bold: true
                    fontSize: 32
                    visible:  dcenowplaying.releasedate =="" ? false: true
                    color:skinStyle.accentColor
                }
            }
            Row{
                id:temporalData
                height: childrenRect.height
                width: childrenRect.width
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                visible: playlist.state === "hidden"
                StyledText {
                    id: updating_time
                    text: dceTimecode.qsCurrentTime
                    fontSize:32
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
                }
            }
        }



    }

    NowPlayingImage {
        id: imageholder
        anchors.top: parent.top
        anchors.topMargin: scaleY(20)
        anchors.right: parent.right
        anchors.rightMargin: scaleX(15)
        visible:false
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
            PropertyChanges {
                target: imageholder
                visible:false
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
}
