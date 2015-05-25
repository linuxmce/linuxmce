import QtQuick 2.2
import QtGraphicalEffects 1.0
/*! This File is designed to be the main layout that can be switched in and out for various forms */
import "../"
Item {
    id:stbDelegate
    height:parent.height
    width: Style.appButtonWidth
    property bool arrow:false
    property bool rotateUp:false
    property bool triggered:false
    property string buttonText:"ipsum"
    property bool currentSelection:false
    property alias fontSize:rowLabel.font.pointSize
    property int buttonRadius:5
    signal activated()
    focus: true
    onActiveFocusChanged: {
        if(activeFocus){
            currentSelection=true
        } else{
            currentSelection=false
        }

    }

    Keys.onReturnPressed: {
        activated()
    }

    Keys.onEnterPressed: {
        activated()
    }

    Rectangle{
        anchors.fill: parent
        color:Style.appcolor_background_list
        opacity: Style.appList_opacity
        radius: buttonRadius
        border.color: "white"
        border.width: stbDelegate.currentSelection ? 2 : 0
    }
    
    StyledText{
        id:rowLabel
        text:buttonText
        font.pointSize:Style.appFontSize_title
        anchors.fill: parent
        elide: Text.ElideRight
    }
    Glow{
        anchors.fill: rowLabel
        radius:16
        samples:24
        spread:0.1
        color:Style.apptext_color_active
        source:rowLabel
        visible:stbDelegate.currentSelection
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
        height: Style.scaleY(3)
        width: height+5
    }
    MouseArea{
        id:ms
        anchors.fill: parent
        onClicked:{ stbDelegate.currentSelection=true; activated()}

    }
}
