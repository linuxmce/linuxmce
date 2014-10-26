// import QtQuick 1.1 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1


Item {
    id:mediaPlaybackBase
    height: pageLoader.height
    width: manager.appWidth

    state:"default"
    Component.onCompleted: manager.setBoundStatus(true)
    property alias metadataComponent:mediaTypeMetaData.sourceComponent
    property alias scrollBarComponent:mediaScrollerTarget.sourceComponent
    property alias controlComponent: controlsLoader.sourceComponent
    property alias playlistSource:playlist.model
    property alias playlistDelegate:playlist.delegate
    property bool enableScrollbar:true

    function togglePlaylistEditor(){
        if(state=="default"){
            state="playlist"
        }else{
            state="default"
        }
    }

    OptionsDisplay {
        id: optionsDisplay
    }



    Rectangle{
        id:metadataSection
        width: parent.width
        height: scaleY(45)
        color: "transparent"

        anchors.verticalCenter: parent.verticalCenter
        clip:false

        Rectangle{
            anchors.fill: parent
            color:"black"
            border.color: "black"
            border.width: 2
            Image {
                id: fill
                source: manager.imagePath+"ui3/appBackground.png"
                anchors.fill:parent
                opacity: .85
            }

        }

        PlaylistTemplateCoverFlow{
            id:playlist
            anchors.centerIn: parent
            visible:false
        }

        NonEPGPlaylist{
            id:current_playlist
            anchors.left: parent.left
            anchors.top: parent.top
        }

        NowPlayingImage{
            id:npImage
            anchors.horizontalCenterOffset: scaleX(-10)
        }

        Loader{
            id:mediaTypeMetaData

            anchors.leftMargin: scaleX(-1)
            height: parent.height*.95
        }




    }
    EditPlaylistBase{
            id:playlistEditor

    }
    StyledText {
        id: updating_time
        text: dceTimecode.qsCurrentTime
        fontSize:32
        anchors.left: parent.left
        anchors.top: metadataSection.bottom
    }

    StyledText {
        id: totalTime
        text: dceTimecode.qsTotalTime
        fontSize:32
        anchors.right: parent.right
        anchors.top: metadataSection.bottom
    }

    Loader{
        id:mediaScrollerTarget
        anchors.top: metadataSection.bottom
        anchors.topMargin: scaleY(1)
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Rectangle{
        id:bottomControls
        width: manager.appWidth
        height: controlsLoader.height
        color: "transparent"
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        Loader{
            id:controlsLoader           
            anchors.centerIn: parent
        }
    }

    Column{
        id:env_controls
        height: childrenRect.height
        width: scaleX(40)
        anchors.bottom: parent.bottom
        anchors.left: parent.left

        AudioControl{
            id:audio_controls
        }
    }

    Rectangle{
        height: 50
        width: 50
        color: "black"
        StyledText{
            anchors.centerIn: parent
            text: optionsDisplay.state == "exposed" ? "Close": "Options"
        }
        anchors.right: home.left
        anchors.bottom: parent.bottom
        MouseArea{
            anchors.fill: parent
            onClicked: optionsDisplay.state=="exposed" ? optionsDisplay.state="hidden": optionsDisplay.state = "exposed"
        }
    }

    HomeButton{
        id:home
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 50
        width: 50
    }

    states: [
        State {
            name: "default"
            PropertyChanges {
                target: npImage
                opacity:1
            }
            PropertyChanges {
                target: current_playlist
                state:"exposed"
            }
            PropertyChanges {
                target: mediaTypeMetaData
                opacity:1
            }
            PropertyChanges{
                target:playlistEditor
                state:"hidden"
            }
            AnchorChanges{
                target: npImage
                anchors.left: undefined
                anchors.horizontalCenter: metadataSection.horizontalCenter
            }
            AnchorChanges{
                target: mediaTypeMetaData

                anchors.left: npImage.right
                }
        },
        State {
            name: "playlist"
            PropertyChanges {
                target: npImage

            }
            PropertyChanges {
                target: mediaTypeMetaData
                opacity:0
            }

            PropertyChanges {
                target: current_playlist
                state:"hidden"
            }
            PropertyChanges{
                target:playlistEditor
                state:"exposed"
            }

            AnchorChanges{
                target: npImage
                anchors.horizontalCenter: undefined
                anchors.right: metadataSection.left
            }
            AnchorChanges{
                target: mediaTypeMetaData
               anchors.left: metadataSection.right

                }
        }
    ]

    transitions: [
        Transition {
            AnchorAnimation{

                duration: 1000
            }

        }
    ]

}
