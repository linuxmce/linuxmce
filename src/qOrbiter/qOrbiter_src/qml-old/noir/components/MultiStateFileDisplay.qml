import QtQuick 2.0


Rectangle {
    id:gridholder
    width: scaleX(85)
    height: scaleY(85)
    clip: true
    anchors.horizontalCenter: parent.horizontalCenter
    color: "transparent"


    Connections{
        target: dcerouter
        onClearAndContinue:runAnimation()
    }

    function runAnimation()
    {
        console.log("Switching Grids ")
        if (gridView.opacity === 1)
        {
            hidegrid1.running = true
            showGrid2.running = true


        }
        else
        {
            showGrid1.running = true
            hidegrid2.running = true
        }

    }




    GridView {
        id: gridView
        //z: 2
        width: scaleX(82)
        height: scaleY(82)

        anchors.centerIn: gridholder
        model:dataModel
        delegate: contactDelegate
        cacheBuffer: 20
        focus: true
        //clip: true
        //contentItem.clip: true
        cellWidth: scaleX(20)
        cellHeight: scaleY(20)
        opacity:1
        scale:1

    }

    GridView {
        id: gridView2
        //z: 2
        width: scaleX(82)
        height: scaleY(82)
        anchors.centerIn: gridholder
        model: dataModel
        delegate: contactDelegate
        cacheBuffer: 20

        //clip: true
        //contentItem.clip: true
        cellWidth: scaleX(20)
        cellHeight: scaleY(20)
        opacity:0
        scale: 0

    }
    ParallelAnimation{
        id:showGrid2
        running:false
        PropertyAnimation {target: gridView2; property: "scale"; to: 1; duration:2000}
        PropertyAnimation {target: gridView2; property: "opacity"; to: 1; duration:750}
    }

    ParallelAnimation{
        id:showGrid1
        running:false
        PropertyAnimation {target: gridView; property: "scale"; to: 1; duration:2000}
        PropertyAnimation {target: gridView; property: "opacity"; to: 1; duration:750}
    }

    ParallelAnimation{
        id:hidegrid2
        running:false
      PropertyAnimation {target: gridView2; property: "scale"; to:0; duration:750}
        PropertyAnimation {target: gridView2; property: "opacity"; to: 0; duration:750}
    }

    ParallelAnimation{
        id:hidegrid1
        running:false
       PropertyAnimation {target: gridView; property: "scale"; to: 0; duration:750}
        PropertyAnimation {target: gridView; property: "opacity"; to:0 ; duration:750}
    }


}
