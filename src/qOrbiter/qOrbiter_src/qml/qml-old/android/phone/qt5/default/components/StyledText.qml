import QtQuick 2.2
import "../../../../skins-common/lib/effects"
    Text {
        property string textLabel:"ipsum"
        property int textHeight
        property int textWidth
        property bool isBold:false
        property bool isItalic:false
        property double fontSize:scaleY(2.5)
        id:labelelement
        text: textLabel
        font.bold: isBold
        font.italic: isItalic
        font.pixelSize: fontSize
       // font.family: appFont

    }



