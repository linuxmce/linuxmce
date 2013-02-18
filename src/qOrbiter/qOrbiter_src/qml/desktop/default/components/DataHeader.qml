import QtQuick 1.1
import Qt.labs.shaders 1.0
import "../components"
import "../../lib/effects"
import "../../lib/handlers"
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
    height: scaleY(5)
    width: manager.appWidth
    anchors.top: parent.top
    anchors.horizontalCenter: parent.horizontalCenter



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
    }
    WaitSpinner {
        id: waitSpinner
        anchors.right: parent.right
    }

    Clock{
        id:screen1time
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
    }

    Row{
        id:data_row
        height: parent.height
        width: childrenRect.width
        anchors.right: parent.right
        anchors.rightMargin: 25
        spacing:scaleX(2)

       Loader{
           id:home
           source: manager.currentScreen !== "Screen_1.qml" ? "HomeButton.qml" : ""
           onSourceChanged: if(home.source !==""){
                                home.item.height =parent.height
                                home.item.width = 25
                            }
       }

        UserListComponent{
            id:user

        }

        Image{
            id:connection_status
            height: parent.height
            fillMode: Image.PreserveAspectFit
            source: manager.connectedState ? manager.imagePath+"ui3/connect_established.png" : manager.imagePath+"ui3/connect_no.png"
        }
        Image{
            id:mediaplayer_status
            height: parent.height
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





    
}
