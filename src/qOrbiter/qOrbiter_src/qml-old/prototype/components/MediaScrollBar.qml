// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Item {
    id:scroller
    width: scaleX(44)
    height: scaleY(3)


    property int slidertimer: (scroll_tab.x / (scroller_transit.width - scroll_tab.width/2)) * dceTimecode.tcTotalTime



    Rectangle{
        id:drag_indicator
        height: scaleY(5)
        width: scaleX(12)
        color:"maroon"
        radius:5
        border.color: "white"
        border.width: 1
        opacity: 0
        StyledText {
            id: drag_label
            text: dceTimecode.dragTime
            anchors.centerIn: parent
            color:"white"
        }
        anchors.bottom: scroll_tab.top
        anchors.topMargin: scaleY(1)
    }

    Item {
        id: scroller_transit

        width: scaleX(44)
        height: scaleY(3)
        anchors.centerIn: parent

        Rectangle{
            anchors.fill: parent
            color:"maroon"
            opacity:.25
            border.color: "white"
            border.width: 1
            radius:5
        }
    }

    Item {
        id: scroller_transit_fill

        width: scroller_transit.x +scroll_tab.x+scroll_tab.width/2
        height: scaleY(3)
        anchors.verticalCenter: scroller_transit.verticalCenter
        anchors.left: scroller_transit.left

        Rectangle{
            anchors.fill: parent
            color:"blue"
            opacity:.25
            radius:5
        }
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
            onPressed: {
                drag_indicator.opacity = 1
            }

            onPositionChanged: if(drag.active)
                               { drag_indicator.opacity = 1
                                   dceTimecode.showDragTime(slidertimer)
                               }
                               else
                               {
                                   drag_indicator.opacity = 0
                               }
            onReleased: {
                drag_indicator.opacity =0
                dceTimecode.finishDragging()
            }
        }
    }


}
