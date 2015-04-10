import QtQuick 2.3
import QtGraphicalEffects 1.0
import AudioVisual 1.0
import "."
import "../"
import "components"
/*! This File is designed to be the main layout that can be switched in and out for various forms */
Item {
    id:layout
    anchors.fill: qmlRoot
    Component{
        id:compList
        RoomSelector {
            id:room_selector
        }
    }

    Rectangle{
        anchors.fill: parent
        color:"black"
    }
    property bool uiOn:true
    function raiseNavigation(raise){
        uiOn=raise;
    }

    QmlPictureFrame {
        id: qmlPictureFrame
        MouseArea{
            anchors.fill: parent
            onClicked: uiOn=!uiOn
        }
    }


    PageLoader {
        id: pageLoader
        anchors{
            top:header.bottom
            left:layout.left
            right:layout.right
            bottom:footer.top
        }
    }

    DefaultHeader {
        id: header
    }

    Footer {
        id: footer

        StyledButton{
            buttonText: manager.sPK_User
            textSize:Style.appFontSize_list
            anchors{
                top:parent.top
                left:parent.left
                bottom:parent.bottom
                margins:5
            }
            width:Style.scaleX(30)
        }

        StyledButton{
            buttonText: roomList.currentRoom
            textSize:Style.appFontSize_list
            anchors{
                top:parent.top
                right:parent.right
                bottom:parent.bottom
                margins:5
            }
            width:Style.scaleX(45)
            onActivated:{
                qmlRoot.createPopup(compList)
            }
        }
    }


}
