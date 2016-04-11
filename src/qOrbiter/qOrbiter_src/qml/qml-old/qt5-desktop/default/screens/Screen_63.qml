import QtQuick 2.2
import org.linuxmce.grids 1.0
import "../components"
StyledScreen {
    id:screen_fiddy_fo
    height: scaleY(100)
    width: scaleX(100)
    Component.onCompleted:setNowPlayingTv()
    focusTarget: playback
    onActiveFocusChanged: {
        if(activeFocus){
            console.log("Screen 63 has focus, transferring it to template.")
            playback.forceActiveFocus()

        }
    }

    Rectangle{
        anchors.fill: parent
        color:"black"
        opacity: parent.activeFocus ? .65 : .25
    }


    MediaPlaybackTemplate{
        id:playback
        focus:true
        onActiveFocusChanged: { console.log("Getting tv playlist") }
    }
    ListView{
        id:playlistView
        clip:true
        anchors{
            top:parent.top
            left:parent.left
            right:parent.right
            bottom:parent.bottom
        }
        spacing: 5
        model:manager.getDataGridModel("tvchan_"+String(manager.iPK_Device),DataGrids.EPG_All_Shows )
      //  onCountChanged: positionViewAtIndex(dcenowplaying.m_iplaylistPosition, ListView.Beginning)


        delegate: Item{
            height: manager.appHeight*.10
            anchors{
                left:parent.left
                right:parent.right
            }
            clip:true

            anchors.margins: 5

//            Rectangle{
//                anchors.fill: parent
//                gradient:style.buttonGradient
//            }

            Rectangle{
                id:fil
                anchors.fill: parent
                color:ms.pressed ? "grey": "black"
                opacity:.50

            }

            StyledText{
                text:program+"\n"+name+timeslot
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 22
                isBold: true
                width: parent.width
                color:"white"
            }
            StyledText{
                text:channel +":"+ sourceid
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 36
                isBold: true
                color: "white"
               // opacity: value == dcenowplaying.m_iplaylistPosition ? 1 :.25
            }

            MouseArea{
                id:ms
                anchors.fill: parent
                onClicked: {
                    manager.gridChangeChannel(channel, channelid)
                }
            }
        }

    }
}
