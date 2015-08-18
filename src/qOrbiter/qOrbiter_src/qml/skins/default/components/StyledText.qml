import QtQuick 2.2
import "../../../."
Text {
    property string textLabel:"ipsum"
    property int textHeight
    property int textWidth  
    property bool isItalic:false
    property double fontSize:Style.appFontSize_description
    property double largeFontSize:Style.appFontSize_title
    property double cellFontSize: Style.appFontSize_list
    horizontalAlignment: Text.AlignHCenter
    id:labelelement
    text: textLabel
    font.pointSize: fontSize
    verticalAlignment:Text.AlignVCenter
   // font.family: appFont.name
    color:Style.apptext_color_active
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
}
