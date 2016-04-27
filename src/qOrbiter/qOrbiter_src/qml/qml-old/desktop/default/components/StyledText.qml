import QtQuick 1.1

Text {
    property string textLabel:"ipsum"
    property int textHeight
    property int textWidth
    property bool isBold:true
    property bool isItalic:false
    property double fontSize:14
    property string textColor:skinStyle.lighttext
    property string shadowColor:"black"
    property bool dropshadow:false
    id:labelelement
    text: textLabel
    font.bold: isBold
    font.italic: isItalic
    font.pointSize: fontSize
    font.family: myFont.name
    color: textColor
    wrapMode: Text.WrapAnywhere

//    DropShadow{
//        id:headerDrop
//        enabled: dropshadow ? true : false

//        height:dropshadow ? parent.height : 0
//        width: dropshadow ? parent.width : 0
//        distance:dropshadow ? 1 : 0
//        color:shadowColor
//    }

}



