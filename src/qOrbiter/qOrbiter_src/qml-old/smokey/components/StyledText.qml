import QtQuick 1.1

Text {
    property string textLabel:"ipsum"
    property int textHeight
    property int textWidth
    property bool isBold:false
    property bool isItalic:false
    property double fontSize:scaleY(2.5)
    horizontalAlignment: Text.AlignHCenter
    id:labelelement
    text: textLabel
    font.pixelSize: fontSize
    font.weight: Font.Light
    font.family: appFont.name
    color:skinStyle.tile_color
}
