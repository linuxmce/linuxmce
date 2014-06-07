import QtQuick 1.1
import "../../../../skins-common/lib/effects"
    Text {
        property string textLabel:"ipsum"
        property int textHeight
        property int textWidth
        property bool isBold:false
        property bool isItalic:false
        property double fontSize:manager.b_orientation ? scaleY(2.5) : scaleX(2.5)
        color:skinStyle.tile_color
        id:labelelement
        text: textLabel
        font.bold: isBold
        font.italic: isItalic
        font.pixelSize: fontSize
        font.weight: Font.Light
        font.family:appFont.name

    }



