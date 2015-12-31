import QtQuick 2.2
import "../../../../../skins-common/qt5/default/components"

Item {
    id:homeNav
    height:style.buttonHeight
    width: manager.appWidth
    visible:manager.currentScreen==="Screen_1.qml"
    anchors.leftMargin: 3
    anchors.topMargin: 3
    StyledButton {
        id:roombt
        buttonText: roomList.currentRoom+" "//+"::"+roomList.currentEA
        height: parent.height
        width:  scaleX(18)
        txtObj.color: "antiquewhite"
        txtObj.isBold: false
        hitArea.onReleased: {
            showRoomSelector();
            console.log(manager.currentScreen)
        }
    }

    Rectangle {
        id: nowPlaying
        height: parent.height
        visible:dcenowplaying.b_mediaPlaying
        anchors.left: roombt.right
        anchors.right: menuBt.left
        anchors.top: parent.top
        color: "black"
        MouseArea{
            anchors.fill: parent
            onPressed: manager.setCurrentScreen(dcenowplaying.qs_screen)
        }

        StyledText {
            id: generaltitle
            width: scaleX(35)
            text:  dcenowplaying.mediatitle === "" ? dcenowplaying.qs_mainTitle : dcenowplaying.mediatitle
            font.bold: true
            elide: "ElideRight"
            smooth: true
            color:"white"
            font.pixelSize: scaleY(3)
            anchors.fill: parent
        }
        StyledText {
            id: updating_time
            text: dceTimecode.qsCurrentTime + " of " + dceTimecode.qsTotalTime
            fontSize:20
            color: "white"
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            visible:nowPlaying.visible
        }

    }

    ListModel {
        id:buttons
        ListElement {
            name : "user"
            text: "User"
        }
        ListElement {
            name : "power"
            text: "Power"
        }
        ListElement {
            name: "sleepingalarms"
            text: "Sleeping Alarms"
        }
        ListElement {
            name : "advanced"
            text: "Advanced"
        }
        ListElement {
            name: "exit"
            text: "Exit"
        }
        function onClick(name) {
            switch (name) {
            case "user":
                info_panel.state="user"
                break;
            case "power":
                info_panel.state="power"
                break;
            case "sleepingalarms":
                manager.setCurrentScreen("Screen_29.qml");
                break;
            case "advanced":
                manager.setCurrentScreen("Screen_44.qml");
                break;
            case "exit":
                manager.closeOrbiter()
                break;
            }
        }
    }
    OptionMenu {
        id: menuBt
        anchors.right: parent.right
        text: "Menu"
        menuItems: buttons
        btHeight: homeNav.height
        menuAlign: "right"
        menuWidth: scaleX(35)
    }

}
