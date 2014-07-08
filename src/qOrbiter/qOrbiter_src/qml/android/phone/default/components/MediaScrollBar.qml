// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Item {
    id:scroller
    anchors{
        left:parent.left
        right:parent.right
        margins: 10
    }
    height: scaleY(8)
    property int slidertimer: (scroll_tab.x / width) * dceTimecode.tcTotalTime
    visible:dceTimecode.tcTotalTime!==0
    GradientFiller{
        fillColor: "green"
        grpRadius: 5
    }

    Rectangle{
        id:drag_indicator
        height: scaleY(8)
        width: scaleY(10)
        opacity: 0
        Text {
            id: drag_label
            text: dceTimecode.dragTime
            anchors.centerIn: parent
            font.pixelSize: scaleY(2)
        }
        anchors.bottom: scroll_tab.top
        anchors.topMargin: scaleY(1)
    }



    Rectangle{
        radius: 5
        color:"white"
        height: parent.height-2
        anchors{
            right:scroll_tab.horizontalCenter
            left:scroller.left
            verticalCenter: parent.verticalCenter
            margins: 2
        }
        opacity: .35
    }

//    Rectangle{
//        id:elapsed_fill
//        height: scroll_tab-1
//        width: (scroller_transit.x + scroll_tab.x)+5
//        anchors.verticalCenter: scroller.verticalCenter
//        anchors.left: scroller_transit.left
//    }



    Image {
        id: scroll_tab
        source: "../img/scroller.png"
        height: scaleY(9)
        width: scaleX(3)
        anchors.verticalCenter: parent.verticalCenter
        x: ( dceTimecode.runningTimer / dceTimecode.tcTotalTime) * scroller.width
        MouseArea{
            anchors.fill: parent
            drag.target: scroll_tab
            drag.axis: Drag.XAxis
            drag.minimumX: 0
            drag.maximumX:  scroller.width
            onPositionChanged: if(drag.active)
                               { drag_indicator.opacity = .75
                                   dceTimecode.showDragTime(slidertimer)  }
                               else
                               {drag_indicator.opacity = 0
                               }
            onReleased: {
                drag_indicator.opacity =0
                dceTimecode.finishDragging()
            }
        }
    }

    StyledText {
        id: np
        text:dceTimecode.qsCurrentTime
        font.pixelSize: scaleY(2.5)

        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        anchors.horizontalCenter: scroller.horizontalCenter
        anchors.verticalCenter: scroller.verticalCenter
        color: "white"
        visible:  dceTimecode.qsCurrentTime ==="0" ? false: true
    }

}
