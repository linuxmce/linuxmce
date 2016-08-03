import QtQuick 2.2
import "../../../."
Text {
    property string textLabel:"ipsum"
    property string preLabel:""
    property int textHeight
    property int textWidth  
    property bool isItalic:false
    property double fontSize:Style.appFontSize_description
    property double largeFontSize:Style.appFontSize_title
    property double cellFontSize: Style.appFontSize_list
    horizontalAlignment: Text.AlignHCenter
    onVisibleChanged: {
        if(!visible)height=0
        else
            height=paintedHeight
    }
    id:labelelement
    text: textLabel   
    font.pixelSize: fontSize+settings.fontSizeMod
    verticalAlignment:Text.AlignVCenter   
    color:Style.apptext_color_active
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere 
}
