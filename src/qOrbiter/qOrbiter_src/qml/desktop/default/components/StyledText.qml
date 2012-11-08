import QtQuick 1.1

Item{
    property string textLabel:"ipsum"
    property int textHeight
    property int textWidth
    property bool isBold:false
    property bool isItalic:false
    property double fontSize:12

    Text {
        id:labelelement
        width: textWidth
        height:textHeight
        text: textLabel
        font.bold: isBold
        font.italic: isItalic
        font.pixelSize: fontSize
        font.family: myFont.source
    }
}


