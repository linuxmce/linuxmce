// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id:scroller
    width: scaleX(44)
    height: scaleY(3)
    color: "transparent"

    property int slidertimer: (scroll_tab.x / scroller_transit.width) * dceTimecode.tcTotalTime



    Rectangle{
        id:drag_indicator
        height: scaleY(6)
        width: scaleY(12)
        opacity: 0
        Text {
            id: drag_label
            text: dceTimecode.dragTime
            anchors.centerIn: parent
        }
        anchors.bottom: scroll_tab.top
        anchors.topMargin: scaleY(1)
    }

    Image {
        id: scroller_transit
        source: "../img/icons/blue.png"
        width: scaleX(44)
        height: scaleY(3)
        anchors.centerIn: parent

    }
    Image {
        id: scroll_tab
        source: "../img/icons/scroller.png"
        height: scaleY(7)
        width: scaleX(4)
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


}
