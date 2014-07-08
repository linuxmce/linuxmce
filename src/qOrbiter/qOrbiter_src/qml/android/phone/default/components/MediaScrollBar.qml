// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Item {
    id:scroller
    width: scaleX(55)
    height: scaleY(8)
    property int slidertimer: (scroll_tab.x / scroller_transit.width) * dceTimecode.tcTotalTime
    visible:dceTimecode.tcTotalTime!==0

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

    Image {
        id: scroller_transit
        source: "../img/blue.png"
        height: parent.height
        width: parent.width
        anchors.centerIn: parent
    }

    Rectangle{
        id:timerFill

    }

    Image{
      id:scroller_fill
      source: "../img/active.png"
      height: scroll_tab.height -1
      width: (scroller_transit.x + scroll_tab.x)+5
      anchors.verticalCenter: scroller_transit.verticalCenter
      anchors.left: scroller_transit.left
    }

    Image {
        id: scroll_tab
        source: "../img/scroller.png"
        height: scaleY(9)
        width: scaleX(3)
        anchors.verticalCenter: scroller_transit.verticalCenter
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

    Text {
        id: np
        text:dceTimecode.qsCurrentTime
        font.pixelSize: scaleY(2.5)
        anchors.bottom: media_scroller.top
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        anchors.horizontalCenter: media_scroller.horizontalCenter
        color: "white"
        visible:  dceTimecode.qsCurrentTime ==="0" ? false: true
    }

}
