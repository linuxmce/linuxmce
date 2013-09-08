import QtQuick 1.1
import Qt.labs.shaders 1.0
import "../components"
import "../../../skins-common/lib/effects"
import "../../../skins-common/lib/handlers"
/*!
 *
 *\brief The Data Header Component
 *
 *\ingroup qml_desktop_default
 *
The data header is a persistent qml header that displays state full information about the current screen.
 *
 *
 */
Item{
    id:dataheader
    height: manager.currentScreen === "Screen_1.qml" ? scaleY(5) : 0
    width: manager.appWidth
    anchors.top: parent.top
    anchors.horizontalCenter: parent.horizontalCenter
    visible: height=== 0 ? false: true //manager.currentScreen === "Screen_1.qml" ? true : false

    Behavior on height{
        PropertyAnimation{
            duration: 750
            property: "height"
            easing.type: Easing.InElastic
            target:dataheader
        }
    }

    DropShadow{
        id:headerDrop
        sourceItem:headerbg
        height: headerbg.height
        width: headerbg.width
        distance:2
        color:"black"
    }

    Image {
        id: headerbg
        source: manager.imagePath+"ui3/bluish_fill.png"
        height: parent.height
        width: parent.width
        opacity: .25
    }
    WaitSpinner {
        id: waitSpinner
        anchors.right: parent.right
    }

    Clock{
        id:screen1time
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        height: parent.height
    }

    Row{
        id:data_row
        height: parent.height
        width: childrenRect.width
        anchors.right: parent.right
        anchors.rightMargin: 25

        MediaListInfoBar{
            id:mediaInfo
            visible:false
        }

        Loader{
            id:home
            source: manager.currentScreen !== "Screen_1.qml" ? "HomeButton.qml" : ""
            height: parent.height
            width: scaleX(6)
        }

        UserListComponent{
            id:user

        }

        Image{
            id:connection_status
            height: visible ? parent.height :0
            fillMode: Image.PreserveAspectFit
            source: manager.connectedState ? manager.imagePath+"ui3/connect_established.png" : manager.imagePath+"ui3/connect_no.png"
            visible:mediaplayer_status.visible
        }
        Image{
            id:mediaplayer_status
            height: visible ? parent.height :0
            fillMode: Image.PreserveAspectFit
            source: manager.imagePath+"ui3/mediaplayer.png"
            StyledText{
                text:!dceplayer.connected ? "!" : dceplayer.mediaBuffer
                font.letterSpacing: 2
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                isBold: true
                color: "black"
            }
            visible: dceplayer.connected

        }

        //        StyledText{
        //            id:connectstatus
        //            text: "Orbiter "+ manager.m_dwPK_Device + " is connected to "+ manager.m_ipAddress
        //            color: "aliceblue"
        //            font.letterSpacing: 2
        //            //            anchors.left: parent.left
        //            //            anchors.leftMargin: scaleX(5)
        //            //            anchors.verticalCenter: headerbg.verticalCenter
        //            isBold: true

        //        }

        //        StyledText{
        //            text:dceplayer.connected ? "Media Player is connected. Buffer:"+ dceplayer.mediaBuffer: "Media Player Disconnected"
        //            font.letterSpacing: 2
        //            //            anchors.left: connectstatus.right
        //            //            anchors.leftMargin: scaleX(5)
        //            //            anchors.verticalCenter: headerbg.verticalCenter
        //            isBold: true
        //        }


    }
    states: [
        State {
            name: "default"
            when: manager.currentScreen==="Screen_1.qml"
            PropertyChanges {
                target: data_row
                spacing:scaleX(2)
            }
            PropertyChanges {
                target: mediaplayer_status
                visible:true
            }
            PropertyChanges {
                target: mediaInfo
                visible:false
            }
        },
        State {
            name: "videos"
            when: manager.currentScreen==="Screen_47.qml" && manager.q_mediaType==="5"
            PropertyChanges {
                target: data_row
                spacing:scaleX(2)
            }
            PropertyChanges {
                target: mediaplayer_status
                visible:false
            }
            PropertyChanges {
                target: mediaInfo
                visible:true
            }
        },
        State {
            name: "audio"
            when: manager.currentScreen==="Screen_47.qml" && manager.q_mediaType==="4"
            PropertyChanges {
                target: data_row
                spacing:scaleX(2)
            }
            PropertyChanges {
                target: mediaplayer_status
                visible:false
            }
            PropertyChanges {
                target: mediaInfo
                visible:true
            }
        }
    ]

}
