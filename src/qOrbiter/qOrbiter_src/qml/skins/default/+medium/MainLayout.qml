import QtQuick 2.4
import QtGraphicalEffects 1.0
import org.linuxmce.screens 1.0
import QtQuick.Controls 1.2
import "."
import "components"
/*! This File is designed to be the main layout that can be switched in and out for various forms */
Item {
    id:layout
    anchors.fill: qmlRoot
    property bool navigtionUp:true

    Connections{
        target: qmlRoot
        onScreenSaverActivated:{
            uiOn=false
            pageLoader.toggleContent(false)

        }
        onResetTimeout:{
            pageLoader.toggleContent(true)
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

    MediaInterface{
        id:mediaPlayer
        anchors.centerIn: parent
        height:parent.height
        width:parent.width
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
            id:actions
            anchors{
                top:parent.top
                left:parent.left
                leftMargin:5
                bottom:parent.bottom
            }

            spacing:5
            width:childrenRect.width
            StyledButton{
                buttonText: qsTr("Home")
                anchors{
                    top:parent.top
                    bottom:parent.bottom
                    margins: 5
                }
                onActivated: manager.currentScreen="Screen_1.qml"
            }

        }
    }

    Footer {
        id: footer

        ScrollRow{
            anchors.fill: parent
            contentHeight:row.height
            contentWidth:row.width

            Row{
                id:row
                spacing:5
                width: children.length*Style.appButtonWidth
                height:footer.height

                StyledButton{
                    buttonText: roomList.currentEA
                    anchors{
                        top:parent.top
                        bottom:parent.bottom
                        margins: 5
                    }
                    onActivated: qmlRoot.createPopup(roomSelector)
                }

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

}
