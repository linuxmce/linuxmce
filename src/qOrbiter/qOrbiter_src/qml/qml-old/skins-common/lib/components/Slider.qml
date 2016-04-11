import QtQuick 2.2
import "../components"
Item{
    id:slider
    height: 120
    width: 300
    property double min:0.0
    property double max:100.0
    property string handleImagePath:""
    property string fillImagePath:""
    property string fillColor:androidSystem.greenStandard
    property string borderColor:"white"
    property double sliderValue: Math.round((calculatedStart / calculatedEnd )* 100)
    property int startingLevel:50
    property int calculatedStart:handle.x + (handle.width/2)
    property int calculatedEnd:ms.drag.maximumX+(handle.width/2)
    signal triggered()
    onSliderValueChanged: parent.val = sliderValue

    Rectangle{
        id:phil
        width: 240
        height: 15
        color:androidSystem.blueStandard
        radius: 5
        border.color: borderColor
        border.width: 1
    }

    Rectangle{
        id:handle
        height: 65
        width: height
        anchors.verticalCenter:phil.verticalCenter
        radius:65
        color:androidSystem.blueHighlight
        opacity: .85

//        onXChanged: {
//            if(ms.drag.active)
//            { controls_loader.active=true; console.log("dragging")}
//            else
//            {  controls_loader.active=false; console.log("not dragging")}
//        }

        MouseArea{
            id:ms
            anchors.fill: handle
            drag.target: handle
            drag.axis: Drag.XAxis
            drag.minimumX:(phil.x-(handle.width/2))
            drag.maximumX: phil.width - (handle.width/2)
            onReleased: {
                setActive(false)
                cmdEntry.sendCommand();
                console.log("released")

            }
            onPressed:{
               setActive(true);
                console.log("pressed")
            }
        }
    }
    Text{
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        text:sliderValue
        color:"white"
        font.pointSize: 18
    }
    
}
