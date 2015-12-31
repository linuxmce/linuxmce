import QtQuick 2.0
import "../components"
Rectangle{
    id:lights
    height: manager.appHeight
    width: manager.appWidth
    color: "transparent"

FloorPlanDisplay{id: lightingfloorplan; anchors.centerIn: parent}

        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }
