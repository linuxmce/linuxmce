import QtQuick 2.0

    Text {
        property string textLabel:"ipsum"
        property int textHeight
        property int textWidth
        property bool isBold:false
        property bool isItalic:false
        property double fontSize:scaleY(2.5)
        property int fontWeight:Font.Light
        id:labelelement
        text: textLabel
        font.bold: isBold
        font.italic: isItalic
        font.pixelSize: fontSize
        font.family: "Roboto"
        font.weight: fontWeight
        height: paintedHeight
        width: paintedWidth
        color:"white"
    }
