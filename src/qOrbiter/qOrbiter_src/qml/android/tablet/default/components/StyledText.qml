import QtQuick 1.1
    Text {
        property string textLabel:"ipsum"
        property int textHeight
        property int textWidth
        property bool isBold:false
        property bool isItalic:false
        property double fontSize:12
        id:labelelement
        text: textLabel
        font.bold: isBold
        font.italic: isItalic
        font.pixelSize: fontSize
        font.family: myFont.name
    }



