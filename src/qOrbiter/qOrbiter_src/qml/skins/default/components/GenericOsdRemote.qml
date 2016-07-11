import QtQuick 2.2
import "../."
Item {
    anchors.fill: parent
    focus:true
    Component.onCompleted: forceActiveFocus()

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
            id:timeCode
            anchors{
                verticalCenter: parent.verticalCenter
                right: parent.right
            }
            text:qsTr("%1 of %2").arg(dceTimecode.qsCurrentTime ).arg(dceTimecode.qsTotalTime)
        }
    }


    MouseArea{
        anchors.fill: parent
        onClicked: qmlRoot.toggleMenu()
    }
}

