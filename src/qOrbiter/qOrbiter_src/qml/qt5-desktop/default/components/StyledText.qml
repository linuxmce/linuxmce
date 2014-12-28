import QtQuick 2.1

Text {

    font.family: myFont.name
    font.pixelSize: fontSize
    font.bold: isBold
    property string textLabel:"ipsum"
    property int textHeight
    property int textWidth
    property bool isBold:false
    property int fontSize:mediumText
    property int headerText:scaleY(5)
    property int mediumText:scaleY(4)
    property int smallText:scaleY(3)
    property double cellFontSize: manager.isProfile ? scaleY(2.8) : scaleY(3.2)
    horizontalAlignment: Text.AlignHCenter
    text: textLabel
     wrapMode: Text.WrapAtWordBoundaryOrAnywhere
}
