import QtQuick 1.1
Item
{ Connections
    {
        target: dataModel
        onProgressChanged:{progress_bar_fill.width = progress_bar.width* ((dataModel.currentCells / dataModel.totalcells))}
        onReady:progress_bar_fill.color = "green"
        //onLoadingStatusChanged:progress_bar_fill.color = dataModel.loadingStatus ? "green" : "red"
    }


    property int barHeight:scaleY(3)
    property int barWidth:gridView.width

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
            height: parent.height
            width: parent.width-1
            color: !dataModel.loadingStatus  ? inactiveColor : activeColor
            anchors.bottom: parent.bottom
            opacity: .25
        }
        ListView{
            id:alphalist
            height: scaleY(4)
            width: gridView.width
            clip:true
           anchors.centerIn: parent
            model:alphabetlist
            orientation:ListView.Horizontal

            delegate:
                Rectangle {
                id:alphabetrect
                height: scaleY(5)
                width: scaleX(4)
                color: "transparent"
                clip:false
                StyledText {
                    id: test
                    text: name
                    font.pixelSize: 18
                    anchors.centerIn: parent
                    color:"aliceblue"

                }
                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: {
                        alphabetrect.scale = 1.5
                    }
                    onExited: {

                        alphabetrect.scale = 1
                    }
                    onClicked: {
                            manager.seekGrid("MediaFile", name)
                        }
                    }
            }
        }

        StyledText {
            id: loaded_cells
            text: dataModel.currentCells
            color: "white"
            font.bold: true
            font.pixelSize: scaleY(4)
           anchors.verticalCenter: progress_bar.verticalCenter
           anchors.right: progress_bar.left

        }

        StyledText {
            id: total_cells
            text: dataModel.totalcells
            color: "grey"
            font.bold: true
            font.pixelSize: scaleY(4)
            anchors.bottom: progress_bar.top
            anchors.left: progress_bar.right
        }

    }
}


