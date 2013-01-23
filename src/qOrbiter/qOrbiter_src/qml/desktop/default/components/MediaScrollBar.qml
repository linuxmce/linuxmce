// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import "../../lib/effects"
/*!
  \class MediaScrollBar
  \brief This is a basic component for scrolling through time in media.
  This component allows a user to drag the media position to a desired location. All of its functions are tied to
  \ref dcetimecode.
  */
Rectangle {
    id:scroller
    width: scaleX(44)
    height: scaleY(3)
    color: "transparent"

    DropShadow{
sourceItem: scroller_transit
anchors.fill: scroller_transit
color:"black"
    }

    property int slidertimer: (scroll_tab.x / scroller_transit.width) * dceTimecode.tcTotalTime

    Rectangle{
        id:drag_indicator
        height: scaleY(5)
        width: scaleY(10)
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
        height: parent.height
        width: parent.width
        anchors.centerIn: parent

    }
    Image {
        id: scroll_tab
        source: "../img/icons/scroller.png"
        height: scaleY(5)
        width: scaleX(2)
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
