import QtQuick 2.2
import "../../../."
Text {
    property string textLabel:"ipsum"
    property string preLabel:""
    property int textHeight
    property int textWidth  
    property bool isItalic:false
    property double fontSize:appStyle.appFontSize_description
    property double largeFontSize:appStyle.appFontSize_title
    property double cellFontSize: appStyle.appFontSize_list
    horizontalAlignment: Text.AlignHCenter
    onVisibleChanged: {
        if(!visible)height=0.1
        else
            height=paintedHeight
    }
    id:labelelement
    text: textLabel   
    font.pointSize: fontSize+settings.fontSizeMod
    verticalAlignment:Text.AlignVCenter   
    color:appStyle.apptext_color_active
    wrapMode: Text.WrapAtWordBoundaryOrAnywhere 
}
