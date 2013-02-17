import QtQuick 1.1
import "../../lib/effects"
    Text {
        property string textLabel:"ipsum"
        property int textHeight
        property int textWidth
        property bool isBold:true
        property bool isItalic:false
        property double fontSize:14
        property string textColor:skinStyle.lighttext
        property string shadowColor:"black"
        id:labelelement
        text: textLabel
        font.bold: isBold
        font.italic: isItalic
        font.pointSize: fontSize
        font.family: myFont.name
        color: textColor
        wrapMode: Text.WrapAnywhere

//        DropShadow{
//            id:headerDrop
//            sourceItem:parent
//            height: parent.height
//            width: parent.width
//            distance:1
//            color:shadowColor
//        }

    }



