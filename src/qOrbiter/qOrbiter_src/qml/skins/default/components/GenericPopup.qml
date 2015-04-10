import QtQuick 2.3

import "../"
Item{
    id:genericPopup
    anchors.fill: parent
    focus: true
    function close(){
        layout.forceActiveFocus();
        genericPopup.destroy()
    }

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
        id:popup_label
        anchors{
            top:parent.top
            horizontalCenter: parent.horizontalCenter
        }
        text:genericPopup.title
        fontSize: Style.appFontSize_title
    }

    Loader{
        anchors{
            top:popup_label.bottom
            left:parent.left
            right:parent.right
            bottom:parent.bottom
        }
        id:contentLoader
        sourceComponent:content
    }
}
