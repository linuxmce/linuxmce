import QtQuick 1.0
Item
{ Connections
    {
        target: dataModel
        onProgressChanged:{progress_bar_fill.height = ((progress_bar.height) * (dataModel.progress / 100))}
        onReady:progress_bar_fill.height = 0
        onLoadingStatusChanged:progress_bar_fill.color = dataModel.loadingStatus ? "green" : "red"
    }


    property int barHeight:scaleY(65)
    property int barWidth:scaleX(2)

    property string borderColor:"white"
    property string internalColor:"transparent"
    property string inactiveColor :"red"
    property string activeColor: "green"
    property int progress: dataModel.progress

    height: barHeight
    width: barWidth

    Rectangle
    { anchors.fill: parent
        id:progress_bar

        color: internalColor
        border.color: borderColor
        border.width: 1


        Rectangle{
            id:progress_bar_fill
            height: 0
            width: parent.width-1
            color: dataModel.loadingStatus === false ? inactiveColor : activeColor
            anchors.bottom: parent.bottom
            opacity: .25
        }

        StyledText {
            id: loaded_cells
            text: dataModel.currentCells
            color: "white"
            font.bold: true
            font.pixelSize: scaleY(4)
           anchors.centerIn: progress_bar
        }

        StyledText {
            id: total_cells
            text: dataModel.totalcells
            color: "grey"
            font.bold: true
            font.pixelSize: scaleY(4)
            anchors.bottom: progress_bar.top
        }

    }
}


