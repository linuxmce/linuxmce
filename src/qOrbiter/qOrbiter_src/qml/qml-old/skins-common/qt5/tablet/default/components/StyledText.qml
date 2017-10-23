import QtQuick 2.2

Text {
    property string textLabel:"ipsum"
    property int textHeight
    property int textWidth
    property bool isBold:false
    property bool isItalic:false
    property double fontSize:manager.isProfile ? scaleY(3.5) :scaleY(3.0)
    property double largeFontSize: manager.isProfile ? scaleY(3) : scaleY(2.5)
    property double headerFontSize: manager.isProfile ? scaleY(4) : scaleY(5)
     property double cellFontSize: manager.isProfile ? scaleY(2.5) : scaleY(2.3)
    horizontalAlignment: Text.AlignHCenter
    id:labelelement
    text: textLabel
    font.family:appFont.name
    font.pointSize: fontSize
    font.bold: isBold
    font.weight: isBold ? Font.Bold : Font.Light
    //font.family: appFont.name
    color:skinstyle.tile_color
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
}
