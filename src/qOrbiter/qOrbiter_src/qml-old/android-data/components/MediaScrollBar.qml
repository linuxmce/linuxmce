// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id:scroller
    width: 55 
    height: 4 
    color: "transparent"
    property int slidertimer: (scroll_tab.x / scroller_transit.width) * dceTimecode.tcTotalTime

    Rectangle{
        id:drag_indicator
        height: 4 
        width: 1700 
        opacity: 0
        Text {
            id: drag_label
            text: dceTimecode.dragTime
            anchors.centerIn: parent
            font.pixelSize: 10
        }
        anchors.bottom: scroll_tab.top
        anchors.topMargin: 1
    }

    Image {
        id: scroller_transit
        source: "../img/blue.png"
        height: parent.height
        width: parent.width
        anchors.centerIn: parent

    }
    Image{
      id:scroller_fill
      source: "../img/active.png"
      height: scroller_transit.height
      width: (scroller_transit.x + scroll_tab.x)+20
      anchors.verticalCenter: scroller_transit.verticalCenter
      anchors.left: scroller_transit.left
    }

    Image {
        id: scroll_tab
        source: "../img/scroller.png"
        height: 48 
        width: 48
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
