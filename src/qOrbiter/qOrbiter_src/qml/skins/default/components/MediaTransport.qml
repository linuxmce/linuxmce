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
        color: Style.appbutton_color
    }

    Rectangle{
        id:bg2
        width: 0+ dragHandle.x + dragHandle.width
        height: bg.height
        radius: height
        color: Style.appbutton_cancel_color
    }

    Item{
        id:drag_indicator
        height: width
        width: Style.scaleX(16*2)
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
            text: roomList.currentEaTimecode.timecodeLength
            anchors.centerIn: parent
            font.pixelSize: Style.scaleY(4)
        }
        anchors.bottom: dragHandle.top
        anchors.topMargin: Style.scaleY(1)

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
                                   roomList.currentEaTimecode.setDragTime(slidertimer)
                               } else{
                                   drag_indicator.opacity = 0
                               }
            onReleased: {
                drag_indicator.opacity =0
                roomList.currentEaTimecode.finishDragging(value);
                // dceTimecode.finishDragging()
            }
        }
    }

    StyledText{
        anchors.centerIn: parent
        color:"black"
        text:roomList.currentEaTimecode.currentTimeCode
    }
}
