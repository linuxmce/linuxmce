import QtQuick 1.0
import "../components"
import "../../lib/components"
Item{
    id:phoneFloorplanLayout
    height: manager.appHeight
    width:manager.appWidth
    property bool rotated:manager.b_orientation
    property string imageDir:""

    Rectangle{
        color: "black"
        anchors.fill: parent
        opacity: .65
    }
    
    ParamManager{
        id:requestParamManager
    }
    
    FloorPlandisplayTemplate{
        id:floorplanPic
    }
    
    
    HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
}
