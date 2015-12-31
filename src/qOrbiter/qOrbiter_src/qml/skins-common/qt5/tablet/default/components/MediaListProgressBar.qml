import QtQuick 2.2

Item
{
    Connections
    {
        target: dataModel
        onProgressChanged:{progress_bar_fill.width = progress_bar.width* ((dataModel.currentCells / dataModel.totalcells))}
        onReady:progress_bar_fill.color = "green"
        //onLoadingStatusChanged:progress_bar_fill.color = dataModel.loadingStatus ? "green" : "red"
    }


    property int barHeight:scaleY(8)
    property int barWidth:manager.appWidth

    property string borderColor:"white"
    property string internalColor:"transparent"
    property string inactiveColor :"red"
    property string activeColor: "green"
    property int progress: dataModel.progress

    height: barHeight
    width: barWidth

    ListModel{
        id:alphabetlist

        ListElement{
            name:"A"
        }

        ListElement{
            name:"B"
        }

        ListElement{
            name:"C"
        }

        ListElement{
            name:"D"
        }

        ListElement{
            name:"E"
        }

        ListElement{
            name:"F"
        }

        ListElement{
            name:"G"
        }

        ListElement{
            name:"H"
        }

        ListElement{
            name:"I"
        }

        ListElement{
            name:"J"
        }

        ListElement{
            name:"K"
        }

        ListElement{
            name:"L"
        }

        ListElement{
            name:"M"
        }
        ListElement{
            name:"N"
        }

        ListElement{
            name:"O"
        }

        ListElement{
            name:"P"
        }

        ListElement{
            name:"Q"
        }

        ListElement{
            name:"R"
        }

        ListElement{
            name:"S"
        }

        ListElement{
            name:"T"
        }

        ListElement{
            name:"U"
        }

        ListElement{
            name:"V"
        }

        ListElement{
            name:"W"
        }

        ListElement{
            name:"X"
        }

        ListElement{
            name:"Y"
        }

        ListElement{
            name:"Z"
        }
    }

    Rectangle{
        anchors.fill: parent
        id:progress_bar

        color: internalColor
        border.color: borderColor
        border.width: 1

        Rectangle{
            id:progress_bar_fill
            height: parent.height
            width: parent.width-1
            color: dataModel.loadingStatus  ? inactiveColor : activeColor
            anchors.bottom: parent.bottom
            opacity: .50
        }


//        StyledText {
//            id: loaded_cells
//            text: dataModel.currentCells
//            color: "white"
//            font.bold: true
//            font.pixelSize: scaleY(4)
//            anchors.verticalCenter: progress_bar.verticalCenter
//            anchors.left: progress_bar.left

//        }

//        StyledText {
//            id: total_cells
//            text: dataModel.totalcells
//            color: "grey"
//            font.bold: true
//            font.pixelSize: scaleY(4)
//            anchors.bottom: progress_bar.top
//            anchors.right: progress_bar.right
//        }

    }
    ListView{
        id:alphalist
        height: scaleY(7)
        width: parent.width
        clip:true
        anchors.centerIn: parent
        model:alphabetlist
        orientation:ListView.Horizontal

        delegate:StyledButton{
            height: scaleY(7)
            width: scaleX(5)
            buttonText: name
            textSize: 35
            onActivated:   {
                //files_view_screen.state="browsing"
                currentSeekLetter = name
                manager.seekGrid("MediaFile", name)
            }
        }
    }
}
