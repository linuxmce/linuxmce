import QtQuick 2.2
import org.linuxmce.grids 1.0
import org.linuxmce.enums 1.0
import org.linuxmce.screens 1.0
import "../."
Item {
    id:gridRoot

    property real fromTime : new Date().getTime() / 1000
    property int timeSpan : 7200
    property real toTime : new Date().getTime() / 1000 + timeSpan
    property variant epgData : manager.getDataGridModel("epgGrid", DataGrids.EPG_Grid, ",,"+fromTime + "," + (fromTime+timeSpan) + ",1")
    property int scaleFactor : 10
    property int markerInterval : 1800

    Component.onCompleted: {
        console.log("onComplete")
        epgData.loadData()
    }

    Component {
        id: progDelegate
        Rectangle {
            property var index
            property int column
            x: getXPos(epgData.get(index, column, "starttime"))
            y: ((column+1) * 50)
            width: (epgData.get(index, column, "endtime") - epgData.get(index, column, "starttime")) / scaleFactor
            height: 50
            color: "lightblue"
            border.color: "black"
            StyledText {
                anchors.fill: parent
                color: "black"
                text: epgData.get(index, column, "program")
            }
        }
    }

    Component {
        id: timeDelegate
        Rectangle {
            property real time
            y: 0
            x: getXPos(time)
            color: "lightBlue"
            border.color: "black"
            width: markerInterval/scaleFactor
            height: Style.appNavigation_panelHeight
            StyledText {
                anchors.leftMargin: 10
                anchors.topMargin: 10
                anchors.fill: parent
                text: getTimeString(parent.time)
            }
        }
    }

    Component {
        id: channelDelegate
        Rectangle {
            property int index
            x: 0
            color: "blue"
            border.color: "black"
            width: 150
            height: 50
            StyledText {
                anchors.leftMargin: 10
                anchors.topMargin: 10
                anchors.fill: parent
                text: epgData.get(0, parent.index, "name")
            }
        }
    }

    function getTimeString(time) {
        return new Date(time*1000).toLocaleTimeString(null, "hh:mm");
    }

    function getXPos(time) {
        return (time-fromTime)/ scaleFactor + 150
    }

    function createGrid() {
        for (var c = 0; c < epgData.getTotalColumns(); c++) {
            console.log("createGrid(): channel :  " + c + ", epgData.count = " + epgData.getTotalRows())
            channelDelegate.createObject(channelCol.contentItem, { "index": c, "y": (c * 50)})
            for (var i = 1; i < epgData.getTotalRows(); i++) {
                progDelegate.createObject(programFlick.contentItem, { "index": i, "column": c })
            }
        }
        for (var j = fromTime; j < toTime; j = j + markerInterval) {
            timeDelegate.createObject(programFlick.contentItem, { "time": j })
        }

    }

    GenericListModel {
        anchors.left: parent.right
        model: epgData
        label: "epg"
        delegate: StyledButton {
            label: program
        }
    }

    Connections {
        target: epgData
        onLoadComplete: {
         console.log("onloadcomplete")
            createGrid()
        }
    }

    ListView {
        id: channelCol
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.topMargin: 50
        z: 100

    }

    Flickable {
        id: programFlick
        anchors.top: parent.top
        anchors.left: channelCol.right
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        contentWidth: 1200
        contentHeight: contentItem.childrenRect.height
        clip: true
        flickableDirection: Flickable.HorizontalFlick
    }

}
