import QtQuick 2.0

    Text {
        property string textLabel:"ipsum"
        property int textHeight
        property int textWidth
        property bool isBold:false
        property bool isItalic:false
        property double fontSize:scaleY(2.5)
        id:labelelement
        text: textLabel       
        font.pixelSize: fontSize
        font.weight: Font.Light
        font.family: appFont.name
        color:"white"
    }
