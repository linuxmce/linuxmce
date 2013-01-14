import QtQuick 1.1


Rectangle{
    id:dragHandle
    height: 30
    width: 15
    radius: 30
    color:"grey"
    smooth:true
    anchors.verticalCenter: dragbar.verticalCenter
    property bool dragging:false
    property int direction: x < dragbar.width/2 ? -1 : 1
    property int distanceCalc:0
    x:(dragbar.width/2) - (radius/2)
    onXChanged: {

        if(dragging){
            var t = Math.round(x-(dragbar.width/2))

            if(t < 0){
                t = t*-1

            }
            console.log(t)
            if(t < 15 ){
                distanceCalc = 1000
            }
            else if (t < 35 )
            {
                distanceCalc = 750
            }
            else if (t < 55)
            {
                distanceCalc = 500
            }

        }
        else
        {
            distanceCalc = 0
        }
    }
    PropertyAnimation{
        id:center_return
        property:"x"
        target: dragHandle
        to:(dragbar.width/2) - (radius/2)
        easing:Easing.InBounce
        duration: 250
    }
    Timer{
        id:cmd_repeat
        interval: distanceCalc
        onTriggered:{
            manager.adjustVolume(direction)
            console.log(distanceCalc+ " "+ direction)
        }

        running:dragging
        repeat: true
    }

    MouseArea{
        id:voltrigger
        anchors.fill: parent
        drag.target: parent

        onPressed:{dragging=true; inactivity_timer.stop()}

        onReleased: {
            inactivity_timer.start()
            dragging = false
            center_return.running = true
        }
        onPressAndHold: {
            inactivity_timer.stop
            dragging = true
            console.log(dragHandle.x)
        }

        drag.axis: Drag.XAxis
        drag.maximumX: dragbar.width - (parent.radius /2)
        drag.minimumX: dragbar.pos.x -( parent.radius /2)
    }
}







