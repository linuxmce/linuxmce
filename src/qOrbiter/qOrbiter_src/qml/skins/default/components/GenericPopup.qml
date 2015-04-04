import QtQuick 2.3

import "../"
Item{
    id:genericPopup
    anchors.fill: parent
    focus: true
    Component.onCompleted: forceActiveFocus()
    Keys.onEscapePressed: {layout.forceActiveFocus(); genericPopup.destroy()}
    property Component content:undefined
    property string title:qsTr("Generic Popup")
    Rectangle{
        anchors.fill: parent
        color:"black"
        opacity: Style.appPanel_opacity
        MouseArea{
            anchors.fill: parent
            onClicked: genericPopup.destroy()
        }
    }

    StyledText{
        anchors{
            top:parent.top
            horizontalCenter: parent.horizontalCenter
        }
        text:genericPopup.title
        fontSize: Style.appFontSize_title
    }

    Loader{
        anchors.centerIn: parent
        anchors.fill: parent
        id:contentLoader
        sourceComponent:content
    }
}
