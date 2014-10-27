import QtQuick 1.1
import "../components"
/*!
  This is screen 54, the stored audio remote screen.
  It has
  *Playlist
    -Playlist Manipulation controls.
  *Media Playback controls
  *Media Image
  *Lighting Control
  *Audio Control
  *Advanced Device Controls

  */
StyledScreen{
    id:stored_audio

    PlaylistPanel {
        id: playlistPanel
    }

    Panel{
        id:metaData
        useHeader: false
        height: parent.height
        width: parent.width*.33
        headerTitle: dcenowplaying.mediatitle
        anchors{
            left:playlistPanel.right
            top:parent.top
            bottom:parent.bottom
        }

        NowPlayingImage{
            id:npImage
            anchors.top:parent.top
            anchors.topMargin: metaData.panelHeaderHeight
            anchors.horizontalCenter: parent.horizontalCenter
            width: scaleY(45)
            height: width
        }

        StyledText{
            width: parent.width
            text: dceTimecode.qsCurrentTime +" of " +dceTimecode.qsTotalTime
            anchors.top: npImage.bottom
            font.pixelSize: 24
            isBold: true
        }

        Metadata_4{
            anchors.bottom: parent.bottom
        }

    }

    ControlDiamond {
        id: controlDiamond
    }

}

