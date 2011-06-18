import QtQuick 1.0



Rectangle {

    id:stage
    Style1 {id:style}
    signal swapStyle()

    width: style.orbiterW
    height: style.orbiterH
    color: style.stage_bg

    AlertBox {
        id:startup
        anchors.centerIn: parent
        color: style.alert_bg
        Text {
            id: starting
            text: "Welcome To Linuxmce"
            anchors.centerIn: parent
            color: style.alert_bg_text
        }
    }
    NotificationBar {id:mnu;  color: style.not_color}

    ControlGrid {id: orbiter_stage; anchors.top: mnu.bottom}

    BottomPanel{id: advanced; color:style.advanced_bg; bottomPanelTextColor: style.advanced_bg_text; anchors.bottom: parent.bottom}

    MouseArea {
        anchors.fill: stage
        onClicked: {
           swapStyle();
            console.log("Ui Swap?")
        }
    }
}

