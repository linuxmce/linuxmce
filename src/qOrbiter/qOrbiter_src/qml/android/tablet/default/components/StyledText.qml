import QtQuick 1.1

Text {
    property string textLabel:"ipsum"
    property int textHeight
    property int textWidth
    property bool isBold:false
    property bool isItalic:false
    property double fontSize:manager.isProfile ? scaleY(3.5) :scaleY(2.5)
    property double largeFontSize: manager.isProfile ? scaleY(3) : scaleY(5)
    horizontalAlignment: Text.AlignHCenter
    id:labelelement
    text: textLabel
    font.pixelSize: fontSize
    font.bold: isBold
    font.weight: isBold ? Font.Bold : Font.Light
    font.family: appFont.name
    color:skinStyle.tile_color
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
}
