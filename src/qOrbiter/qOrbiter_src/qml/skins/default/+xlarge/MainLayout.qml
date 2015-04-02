import QtQuick 2.3
import QtGraphicalEffects 1.0
import AudioVisual 1.0
import DceScreenSaver 1.0
import QtMultimedia 5.0
import "."
import "components"
/*! This File is designed to be the main layout that can be switched in and out for various forms */
Item {
    id:layout
    anchors.fill: qmlRoot
    Rectangle{
        anchors.fill: parent
        color:"black"
    }
    property bool uiOn:true

    Component.onCompleted: forceActiveFocus()
    focus:true
    activeFocusOnTab: false
    onActiveFocusChanged: {
        console.log("Layout has focus ? "+ activeFocus)
    }

    Keys.onTabPressed: {
        console.log("Shift Focus")
        if(layout.activeFocus){
            header.forceActiveFocus()
        } else if (header.activeFocus){
            footer.forceActiveFocus()
        } else {
            layout.forceActiveFocus()
        }
    }

    MouseArea{
        anchors.fill: parent
        onClicked: uiOn=!uiOn
    }

    QmlPictureFrame {
        id: qmlPictureFrame
    }

        DceScreenSaver{
            id:glScreenSaver
            visible: true
            height:parent.height
            width:parent.width
            enableDebug: true
            interval:60*1000
            useAnimation: true
            //onDebugInfoChanged: console.log(debugInfo)
            active:manager.connectedState
            requestUrl:manager.m_ipAddress
            Component.onCompleted: {
                glScreenSaver.setImageList(manager.screensaverImages)
                console.log("Orbiter Consume Screensaver images")
                console.log("Orbiter counts " + glScreenSaver.pictureCount)
            }

            Connections{
                target:manager
                onScreenSaverImagesReady:{
                    glScreenSaver.setImageList(manager.screensaverImages)
                    console.log("Orbiter Consume Screensaver images")
                    console.log("Orbiter counts " + glScreenSaver.pictureCount)
                }
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    uiOn=!uiOn
                }
            }

        }

    PageLoader {
        id: pageLoader
        anchors{
            top:header.bottom
            left:layout.left
            right:layout.right
            bottom:layout.bottom
        }
    }

    DefaultHeader {
        id: header
        focus:true

        StyledText{
            id:orbiterId
            anchors{
                left:parent.left
                verticalCenter: parent.verticalCenter
                leftMargin:15
            }
            text:qsTr("Orbiter %1").arg(manager.iPK_Device)
        }



        Row{
            id:topControls
            focus:true
            anchors{
                right:parent.right
                top:parent.top
                bottom:parent.bottom
                margins: Style.buttonSpacing
            }
            width: Style.scaleX(30)
            spacing: Style.buttonSpacing
            LargeStyledButton{
                buttonText: qsTr("Home")
                onActivated: manager.setCurrentScreen("Screen_1.qml")
            }

            LargeStyledButton{
                buttonText:manager.sPK_User
            }

        }
    }

    Footer {
        id: footer

        focus:true
        onActiveFocusChanged:{
            if(activeFocus)scenarioList.forceActiveFocus()
        }

        ListView{
            id:scenarioList
            focus:true
            onActiveFocusChanged: {
                if(activeFocus)
                    footer.activated=true
                else
                    footer.activated = false
            }
            anchors{
                top:parent.top
                left:parent.left
                right:parent.right
                bottom:parent.bottom
                margins: 5
            }
            spacing:5
            orientation: ListView.Horizontal
            model:qmlRoot.scenarios
            delegate:  LargeStyledButton{
                buttonText:name
                arrow:true
            }
        }
    }

}
