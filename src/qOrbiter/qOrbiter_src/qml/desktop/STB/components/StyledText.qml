import QtQuick 1.1

Text {
    width: paintedWidth
    height: paintedHeight
    font.family: myFont.name
    font.pixelSize: fontSize   
    property int fontSize:mediumText
    property int headerText:scaleY(5)
    property int mediumText:scaleY(4)
    property int smallText:scaleY(3)

}
