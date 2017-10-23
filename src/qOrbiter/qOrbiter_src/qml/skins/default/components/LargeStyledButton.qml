import QtQuick 2.2
import QtGraphicalEffects 1.0
/*! This File is designed to be the main layout that can be switched in and out for various forms */
import "../"
Item {
    id:stbDelegate
    height:parent.height
    width: appStyle.appButtonWidth
    property bool arrow:false
    property bool rotateUp:false
    property bool triggered:false
    property string buttonText:"ipsum"
    property bool currentSelection:false
    property alias fontSize:rowLabel.font.pointSize
    property alias phil:filler
    property int buttonRadius:5
    property bool useAnimation:true
    signal activated()
    focus: true

    onActivated: {
        if(useAnimation){
            confirm_animation.start()
        }
    }

    SequentialAnimation{
            id:confirm_animation

        ParallelAnimation{
            PropertyAnimation{
          target:phil
                property: "opacity"
                to:1
                duration: appStyle.transition_animationTime
            }

            PropertyAnimation {
                target:phil
                property:"color"
                to: appStyle.appbutton_confirm_color
                duration: appStyle.transition_animationTime
            }
        }

        ParallelAnimation{

            PropertyAnimation{
                target:phil
                property: "opacity"
                to:appStyle.appList_opacity
                duration: appStyle.transition_animationTime
            }

            PropertyAnimation {
                target:phil
                property:"color"
                to: appStyle.appcolor_background_list
                duration: appStyle.transition_animationTime
            }
        }
    }
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
        id:filler
        anchors.fill: parent
        color:  appStyle.appcolor_background_list
        opacity: appStyle.appList_opacity
        radius: buttonRadius
        border.color: "white"
        border.width: stbDelegate.currentSelection ? 2 : 0
    }
    
    StyledText{
        id:rowLabel
        text:buttonText
        font.pointSize:appStyle.appFontSize_title
        anchors.fill: parent
        elide: Text.ElideRight
    }
    Glow{
        anchors.fill: rowLabel
        radius:16
        samples:24
        spread:0.1
        color:appStyle.apptext_color_active
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
        height: appStyle.scaleY(3)
        width: height+5
    }
    MouseArea{
        id:ms
        anchors.fill: parent
        onClicked:{ stbDelegate.currentSelection=true; activated()}

    }
}
