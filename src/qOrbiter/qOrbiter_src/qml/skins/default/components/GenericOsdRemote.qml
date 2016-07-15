import QtQuick 2.2
import "../."
import org.linuxmce.enums 1.0
import enums.media 1.0
Item {
    anchors.fill: parent
    focus:true

    Component.onCompleted: {
        manager.setBoundStatus(true);
    }

   Keys.onPressed: {
       switch(event.key){
       case Qt.Key_S:manager.stopMedia(); event.accepted = true; break;
       case Qt.Key_P:manager.pauseMedia(); event.accepted = true; break;
       default:event.accepted = false; event.accepted = true; break
       }
   }

    Item{
        id:header
        anchors{
            top:parent.top
            left:parent.left
            right:parent.right
        }

        height: Style.scaleY(12)

        Rectangle{
            id:headerBg
            anchors.fill: parent
            color:Style.appcolor_navigationBackgroundColor
            opacity: Style.appPanel_opacity
        }

        StyledText{
            id:mediaTitle1
            anchors{
                verticalCenter: parent.verticalCenter
                left:parent.left
            }
            text:dcenowplaying.qs_mainTitle
        }

        StyledText{
            id:mediatype
            anchors{
                verticalCenter: parent.verticalCenter
                left:mediaTitle1.right
                right:timeCode.left
            }
            text:MediaHelper.translateType(manager.i_current_mediaType)
        }

        StyledText{
            id:timeCode
            anchors{
                verticalCenter: parent.verticalCenter
                right: parent.right
            }
            text:qsTr("%1 of %2").arg(roomList.currentEaTimecode.currentTimeCode).arg(roomList.currentEaTimecode.totalTimeCode)
        }
    }


    MouseArea{
        anchors.fill: parent
        onClicked: uiOn=true
    }
}

