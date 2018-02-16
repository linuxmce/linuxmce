import QtQuick 2.2
import org.linuxmce.enums 1.0
import org.linuxmce.screens 1.0
import QtGraphicalEffects 1.0
import "../."
Item{
    id:mediaScrollerTarget
    clip:false
    property int slidertimer: (dragHandle.x / width) * roomList.currentEaTimecode.timecodeLength
    Rectangle{
        id:bg
        width: parent.width
        height: parent.height
        radius: height
        color: appStyle.appbutton_color
    }

    Rectangle{
        id:bg2
        width: 0+ dragHandle.x + dragHandle.width
        height: bg.height
        radius: height
        color: appStyle.appbutton_cancel_color
    }

    Item{
        id:drag_indicator
        height: width
        width: appStyle.scaleX(8)
        opacity: 0
        //  headerTitle: "Seeking to"
        // useHeader: false
        x:dragHandle.x
        onXChanged: {
            if(opacity===1){

            }
        }

        StyledText {
            id: drag_label
            text: roomList.currentEaTimecode.dragString
            anchors.centerIn: parent
            font.pixelSize: appStyle.scaleY(4)
        }
        anchors.bottom: dragHandle.top
        anchors.topMargin: appStyle.scaleY(1)

    }

    Rectangle{
        id:dragHandle
        height: parent.height*1.15
        anchors.verticalCenter: bg.verticalCenter
        width: height
        radius: height
        x: dg.drag.active ? dg.x : ( roomList.currentEaTimecode.timecodePosition / roomList.currentEaTimecode.timecodeLength) * bg.width

        MouseArea{
            id:dg
            anchors.fill: parent
            drag.target: dragHandle
            drag.axis: Drag.XAxis
            drag.minimumX: 0
            drag.maximumX:  bg.width
            onPositionChanged: if(drag.active){
                                   drag_indicator.opacity = 1
                                 roomList.currentEaTimecode.dragTime = slidertimer
                               } else{
                                   drag_indicator.opacity = 0
                               }
            onReleased: {
                drag_indicator.opacity =0
                roomList.currentEaTimecode.finishDragging(slidertimer);

            }
        }
    }

    StyledText{
        anchors.centerIn: parent
        color:"black"
        text:roomList.currentEaTimecode.currentTimeCode
    }
}
