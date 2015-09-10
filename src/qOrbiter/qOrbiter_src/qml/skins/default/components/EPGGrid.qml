import QtQuick 2.2
import org.linuxmce.grids 1.0
import org.linuxmce.enums 1.0
import org.linuxmce.screens 1.0
import "../."
Item {
    id:gridRoot

    property real fromTime : new Date().getTime() / 1000
    property int timeSpan : 7200  // total timespan to display on screen at any time
    property real toTime : new Date().getTime() / 1000 + timeSpan
    property variant epgData : manager.getDataGridModel("epgGrid", DataGrids.EPG_Grid, ",,"+fromTime + "," + (fromTime+timeSpan) + ",1")
    property int scaleFactor : 10  // factor to divide time unit with to get actual pixels
    property int markerInterval : 1800 // time interval (in seconds) for time markes in top row
    property int cellHeight : 50 // cell (row) height in pixels
    property string broadcastSource : "" // the broadcast source, empty means all

    Component.onCompleted: {
        console.log("onComplete")
        epgData.loadData()
    }

    Component {
        id: progDelegate
        StyledButton {
            property var index
            property int column
            property string program
            property real starttime
            property real endtime
            property string info
            x: getXPos(starttime)
            y: ((column+1) * cellHeight)
            width: (endtime - starttime) / scaleFactor
            height: cellHeight
            buttonColor: "lightblue"
            label: program
            onActivated: {
                showInfo(program, info)
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
            height: cellHeight
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
        StyledButton {
            property int index
            property int channel
            property string channelid
            property string name
            x: 0
            width: 150
            height: cellHeight
            buttonColor: "blue"
            label: name
            onActivated: {
                manager.gridChangeChannel(channel, channelid)
            }
        }
    }

    function getTimeString(time) {
        return new Date(time*1000).toLocaleTimeString(null, "hh:mm");
    }

    function getXPos(time) {
        return (time-fromTime)/ scaleFactor + 150
    }

    function addChannel(c) {
//        console.log("addChannel(): channel :  " + c + ", epgData.count = " + epgData.getTotalRows())
        channelDelegate.createObject(channelCol.contentItem, { "index": c, "y": (c * cellHeight),
                                     "name": epgData.get(0, c, "name"),
                                     "channel": epgData.get(0, c, "channel"),
                                     "channelid": epgData.get(0, c, "channelid")
                                     });
    }

    function addProgram(c, i) {
        progDelegate.createObject(programFlick.contentItem, { "index": i, "column": c,
                                     "starttime": epgData.get(i, c, "starttime"),
                                     "endtime": epgData.get(i, c, "endtime"),
                                     "program": epgData.get(i, c, "program"),
                                     "seriesid": epgData.get(i, c, "seriesid")
                                     });
    }

    function createGrid() {
        for (var c = 0; c < epgData.getTotalColumns(); c++) {
            addChannel(c);
            for (var i = 1; i < epgData.getTotalRows(); i++) {
                addProgram(c, i);
            }
        }
        for (var j = fromTime; j < toTime; j = j + markerInterval) {
            timeDelegate.createObject(programFlick.contentItem, { "time": j })
        }

    }

    function showInfo(title, info) {
        infoPopup.info = info
        infoPopup.title = title
        infoPopup.visible = true
    }

    GenericPopup {
        id: infoPopup
        visible: false
        z: 200
        property string info
        anchors.fill: parent
        anchors.margins: 30
        content: StyledText {
                anchors.fill: parent
                text: info
            }
    }

    // Dummy: we need this to trigger loading of the actual data until I figure out how to trigger it
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
        anchors.topMargin: cellHeight
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
