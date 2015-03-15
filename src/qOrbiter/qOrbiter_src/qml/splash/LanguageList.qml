import QtQuick 2.3
import "."

Item{
    id:languageList

    Rectangle{
        anchors.fill: languageList
        color: Style.appcolor_background_medium
    }
    ListView{
        anchors.fill: languageList
        model:languages
        delegate: Text{
            text:name
        }
    }
    
    MouseArea{
        anchors.fill: parent
        onClicked: om_root.state="switch"
    }
}
