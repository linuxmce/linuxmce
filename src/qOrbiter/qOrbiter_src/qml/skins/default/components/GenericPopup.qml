import QtQuick 2.2

import "../"
Item{
    id:genericPopup
    anchors.fill: parent
    focus: true
    Connections{ target:qmlRoot ; onClearDialogs:{  close(); }  }
    Connections{ target:manager; onScreenChange: { close();  } }
    function close(){
        layout.forceActiveFocus();
        genericPopup.destroy()
    }
    Component.onCompleted: forceActiveFocus()
    Keys.onEscapePressed: {layout.forceActiveFocus(); genericPopup.destroy()}
    onActiveFocusChanged: if(activeFocus) contentLoader.item.forceActiveFocus()
    property Component content:undefined
    property string title:qsTr("Generic Popup")
    Rectangle{
        anchors.fill: parent
        color:"black"
        opacity: .65
        MouseArea{
            anchors.fill: parent
            onClicked: genericPopup.destroy()
        }
    }

    Rectangle{
        id:hdr_bg
        color:Style.appcolor_background_medium
        anchors{
            top:parent.top
            left:parent.left
            right:parent.right
        }
        height: Style.appNavigation_panelHeight
    }

    StyledText{
        id:popup_label
        width:parent.width
        anchors.centerIn: hdr_bg
        text:genericPopup.title
        fontSize: Style.appFontSize_title
    }

    Loader{
        anchors{
            top:hdr_bg.bottom
            left:parent.left
            right:parent.right
            bottom:parent.bottom            
        }
        id:contentLoader
        sourceComponent:content
    }
}
