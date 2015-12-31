import QtQuick 1.1

Text {
    property string textLabel:"ipsum"
    property int textHeight
    property int textWidth
    property bool isBold:true
    property bool isItalic:false
    property double fontSize:14
    property string textColor:mystyle.lighttext
    id:labelelement
    text: textLabel
    font.bold: isBold
    font.italic: isItalic
    font.pointSize: fontSize
    font.family: fontstyle.name
    color: textColor
    wrapMode: Text.WrapAnywhere
}
