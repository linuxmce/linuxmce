import QtQuick 1.0

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
        font.family: appFont.name       
        color:"white"
    }
