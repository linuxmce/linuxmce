import QtQuick 2.3
import QtGraphicalEffects 1.0
/*! This File is designed to be the main layout that can be switched in and out for various forms */
import "../"
Item {
    id:stbDelegate
    height:parent.height
    width: Style.scaleX(15)
    property bool arrow:false
    property bool rotateUp:false
    property string buttonText:"ipsum"
    signal activated()
    focus: true
    onActiveFocusChanged: {
        if(activeFocus)
            console.log(buttonText+" button has focus")
    }

    Rectangle{
        anchors.fill: parent
        color:Style.appcolor_background_list
        opacity: Style.appList_opacity
        radius: 5
    }
    
    StyledText{
        id:rowLabel
        text:buttonText

        font.pointSize:Style.appFontSize_title
        anchors.fill: parent
    }
    Glow{
        anchors.fill: rowLabel
        radius:16
        samples:24
        spread:0.1
        color:Style.apptext_color_active
        source:rowLabel
        visible:stbDelegate.activeFocus
    }
    Image {
        id: arrow_icon
        source: "../images/vertical_arrow.png"
        visible:arrow
        rotation: ms.pressed ? rotateUp? 90 : 180 :-90
        anchors{
            verticalCenter: parent.verticalCenter
            right:parent.right
        }
        height: parent.height /2
    }
    MouseArea{
        id:ms
        anchors.fill: parent
        onPressed: activated()
    }
}
