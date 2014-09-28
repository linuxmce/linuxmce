import QtQuick 1.1
import "../../../../skins-common/lib/effects"
    Text {
        property string textLabel:"ipsum"
        property int textHeight
        property int textWidth
        property bool isBold:false
        property bool isItalic:false
        property double fontSize:scaleY(3.5)
        property int headerText:scaleY(10)
        property int paraText:scaleY(6)
        property int listViewText:scaleY(7)
        id:labelelement
        text: textLabel       
        font.pointSize: fontSize
        font.family: myFont.name
        font.weight: Font.Light
    }



