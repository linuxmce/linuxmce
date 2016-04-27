import QtQuick 1.1

import AudioVisual 1.0
import "../effects"
import "../components"
import "../animation"
import "../../../skins-common/lib/handlers"
import "../js/ComponentLoader.js" as MyJs


Rectangle{
    id:rectButton
    height: textElement.height > 34 ? textElement.height : 34
    width: scaleX(9)
    color: "transparent"
    property string text
    Image {
        id: bgimage
        source: manager.imagePath+"ui3/green_button.png"
        anchors.fill: parent
    }
    StyledText{
        id:textElement
        text:rectButton.text
        color: "white"
        anchors.centerIn: parent
    }
    
}
