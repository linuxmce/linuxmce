import QtQuick 2.2
import QtGraphicalEffects 1.0
import AudioVisual 1.0
import org.linuxmce.screens 1.0
import "."
import "components"
/*! This File is designed to be the main layout that can be switched in and out for various forms */
Item {
    id:layout
    anchors.fill: qmlRoot
    property alias loader:pageLoader
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

        Component{
            id:roomSelector
            RoomSelector{

            }
        }

        FocusRow{
            anchors.fill: parent
            StyledButton{
                buttonText: qsTr("Home")
                anchors{
                    top:parent.top
                    bottom:parent.bottom
                    margins: 5
                }
                onActivated: manager.currentScreen="Screen_1.qml"
            }
            StyledButton{
                buttonText: roomList.currentRoom
                anchors{
                    top:parent.top
                    bottom:parent.bottom
                    margins: 5
                }
                onActivated: qmlRoot.createPopup(roomSelector)
            }
        }

    }

    Footer {
        id: footer

        FocusRow{

            anchors.fill: parent

            StyledButton{
                buttonText: qsTr("Sleeping Menu")
                anchors{
                    top:parent.top
                    bottom:parent.bottom
                    margins: 5
                }
                onActivated: manager.currentScreen=Screens.Sleeping
            }
            StyledButton{
                anchors{
                    top:parent.top
                    bottom:parent.bottom
                    margins: 5
                }
                buttonText: qsTr("Advanced Menu")
                onActivated: manager.currentScreen = Screens.AdvancedOptions
            }
            StyledButton{

                anchors{
                    top:parent.top
                    bottom:parent.bottom
                    margins: 5
                }

                buttonText: qsTr("Power")
                onActivated: manager.currentScreen=Screens.Power
            }
            StyledButton{
                anchors{
                    top:parent.top
                    bottom:parent.bottom
                    margins: 5
                }
                buttonText: qsTr("ToggleUI")
                onActivated: qmlRoot.screenSaverActivated()
            }
        }
    }

}
