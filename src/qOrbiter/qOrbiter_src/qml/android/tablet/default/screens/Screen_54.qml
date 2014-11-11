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
    navigation: "HeaderControls.qml"
    Panel{
        id:mainPanel
        useHeader: false

        anchors{
            top:parent.top
            left:parent.left
            bottom:parent.bottom
            right:parent.right
            margins: 10
        }
        headerTitle: dcenowplaying.mediatitle

        PlaylistPanel {
            id: playlistPanel
        }

        Item{
            id:centerLayout
            anchors{
                top:parent.top
                left:playlistPanel.right
                bottom:parent.bottom
            }
            width: parent.width*.33

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
}

