import QtQuick 1.0

Rectangle {

    id:stage
    Style1 {id:style}

    width: style.orbiterW
    height: style.orbiterH

    AlertBox {
        id:startup
        anchors.centerIn: parent
        Text {
            id: starting
            text: "Welcome To Linuxmce"
            anchors.centerIn: parent
        }
    }
    NotificationBar {id:mnu;  color: style.not_color}

    ControlGrid {id: orbiter_stage; anchors.top: mnu.bottom}

    BottomPanel{id: advanced; anchors.bottom: parent.bottom}

    MouseArea {
        anchors.fill: mnu
        onClicked: {
            Qt.quit();
        }
    }
}
