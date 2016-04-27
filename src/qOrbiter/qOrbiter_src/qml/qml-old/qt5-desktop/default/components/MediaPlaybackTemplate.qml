import QtQuick 2.1
import "../../../skins-common/qt5-lib/handlers"
Item {
    id:template
    state:"info"
    anchors{
        top:pageLoader.top
        left:pageLoader.left
        right:pageLoader.right
        bottom:pageLoader.bottom
    }
    focus:true
    onActiveFocusChanged: {
        if(activeFocus){
            console.log("Media Playback Template recieved active focus for context screen "+manager.currentScreen)
            // console.log(dcenowplaying.qs_screen)
            playlist.forceActiveFocus()
            dceTimecode.restart()
        }
    }

    Connections{
        target:qmlroot
        onShowMetaData:{
            if(template.state==="hidden")
                template.state="info"
            else
                template.state="hidden"
        }
    }

    Item{
        id:infoHdr
        anchors.top: template.top
        anchors.right: parent.right
        anchors.left: parent.left
        height: scaleY(8)
        anchors.rightMargin: appStyle.margins
        anchors.leftMargin: appStyle.margins

        Rectangle{
            id:titleRect
            color:appStyle.primaryLightColor
            anchors{
                margins: 5
                top:parent.top
                bottom:parent.bottom
            }
            GradientFiller{
                opacity: .65
                fillColor:"black"
            }

            radius: 2
            width: scaleX(35)
            StyledText{
                id:titleData
                text: dcenowplaying.qs_mainTitle
                anchors.centerIn: parent
                fontSize: headerText
                color: "white"
                width:parent.width
                elide:Text.ElideRight
            }
        }
        Rectangle{
            id:subTitleRect
            color:appStyle.primaryLightColor
            anchors{
                margins: 5
                top:parent.top
                bottom:parent.bottom
                left:titleRect.right
            }
            GradientFiller{
                opacity: .65
                fillColor:"black"
            }

            radius: 2
            width: scaleX(35)
            StyledText{
                id:subTitleData
                text: dcenowplaying.qs_subTitle
                anchors.centerIn: parent
                fontSize: headerText
                color: "white"
                width:parent.width
                elide:Text.ElideRight
            }
        }
        Rectangle{
            id:temporalRect
            color:appStyle.primaryLightColor
            anchors{
                margins: 5
                top:parent.top
                bottom:parent.bottom
                right:parent.right
                left:subTitleRect.right
            }
            width: scaleX(20)
            GradientFiller{
                opacity: .75
                fillColor:"black"
            }
            Row{
                id:temporalData
                anchors{
                    top:parent.top
                    left:parent.left
                    right:parent.right
                }
                height:totalTime.height
                width: parent.width
                spacing:scaleX(2)

                StyledText {
                    id: updating_time
                    text: dceTimecode.qsCurrentTime
                    fontSize:32
                    color:"white"

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
                    color:"white"
                }
            }
        }
    }


    NowPlayingImage {
        id: imageholder
        anchors.top: infoHdr.bottom
        anchors.topMargin: scaleY(9)
        anchors.right: parent.right
        anchors.rightMargin: scaleX(15)
    }

    Item{
        id:mediaInformation
        width:parent.width/2
        visible:false
        anchors{
            top:imageholder.bottom
            bottom:parent.bottom
        }
        Rectangle{
            color: "black"
            opacity: .85
            anchors.fill: parent
        }

        //        Loader{
        //            id:textCol
        //            anchors{
        //                top:temporalData.bottom
        //                left: parent.left
        //                right:parent.right
        //                bottom:parent.bottom
        //            }
        //            source:"Metadata"+manager.i_current_mediaType+".qml"
        //            visible:template.state === "info"
        //        }
        clip:false
    }

    Keys.onVolumeDownPressed: manager.adjustVolume("-1")
    Keys.onVolumeUpPressed:  manager.adjustVolume("+1")

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

        case Qt.Key_Minus: /* Minus Sign */
            manager.adjustVolume(-1)
            break;
        case Qt.Key_T:
            if(template.state==="info")
                template.state="hidden"
            else
                template.state = "info"
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

    TemplateListView{
        id:playlist
        anchors{
            top:infoHdr.bottom
            bottom:parent.bottom
            margins: scaleX(1)

        }
    }


    states: [
        State {
            name: "info"

            PropertyChanges {
                target: playlist
                state:"showing"
            }
            PropertyChanges {
                target: imageholder
                visible:true
            }
            PropertyChanges {
                target: infoHdr
                visible:true
            }
            AnchorChanges{
                target: mediaInformation
                anchors.left:undefined
                anchors.right:template.right

            }

        }, State {
            name: "hidden"
            PropertyChanges {
                target: playlist
                state:"hidden"
            }
            PropertyChanges {
                target: imageholder
                visible:false
            }
            PropertyChanges {
                target: infoHdr
                visible:false
            }
            AnchorChanges{
                target: mediaInformation
                anchors.right:undefined
                anchors.left:template.right
            }

        }
    ]

    MouseArea{
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        onClicked: {
            console.log(ftr.isActive)
            if(ftr.isActive){
                pageLoader.forceActiveFocus()
                ftr.state="hidden"
            }
            else
            {
                ftr.forceActiveFocus()
            }
        }
    }
}
