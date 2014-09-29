import QtQuick 1.1
import "../../../../skins-common/lib/effects"
Text {

    property string textLabel:"ipsum"
    property int textHeight
    property int textWidth
    property bool isBold:false
    property bool isItalic:false
    property double fontSize:manager.isProfile ? scaleY(2.5) : scaleX(2.5)
    property double headerSize:scaleY(6)
    property double paraSize:manager.isProfile ? scaleY(4) : scaleY(6)
    property double regSize:manager.isProfile ? scaleY(3) : scaleY(5)
    color:appStyle.tile_color
    id:labelelement
    text: textLabel
    font.bold: isBold
    font.italic: isItalic
    font.pixelSize: fontSize
    font.weight: Font.Light
    font.family:appFont.name

    states: [
        State {
            name: "header"
            PropertyChanges {
                target: labelelement
                font.pixelSize: headerSize
                font.bold: true
                font.weight: Font.DemiBold
                font.capitalization: Font.AllUppercase
            }
        },
        State {
            name: "para"
            PropertyChanges {
                target: labelelement
                font.pixelSize: paraSize
                font.bold: false
                font.weight: Font.Light
            }
        }
        ,
        State {
            name: "reg"
            PropertyChanges {
                target: labelelement
                font.pixelSize: regSize
                font.bold: false
                font.weight: Font.Normal
            }
        }
    ]

}



