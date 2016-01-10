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
    property string broadcastSource : "" // the broadcast source to show channels from, empty means all
    property var callbacks : ({})
    property variant infoPopup

    Component.onCompleted: {
        console.log("onComplete")
        epgData.loadData()
    }

    Connections {
        target: manager
        onDceCommandCompleted: {
            console.log("dceCommandCompleted cbno = " + callback + ", responseParams = " + params[10]);
            var cb = callbacks[callback];
            if (cb) {
                if (cb.type == 'SR') {
                    cb.progDel.recording = 'O'
                    infoPopup.recording = 'O'
                    infoPopup.recordid = params[10]
                } else if (cb.type == 'CR') {
                    cb.progDel.recording = ''
                    infoPopup.recording = ''
                    infoPopup.recordid = 0
                }
                callbacks[callback] = null;
            }
        }
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
            property string recording
            x: getXPos(starttime)
            y: (column * cellHeight)
            width: (endtime - starttime) / scaleFactor
            height: cellHeight
            buttonColor: recording == 'O' ? "lightgreen" : "lightblue"
            label: program
            onActivated: {
                showInfo(epgData.get(0, column, "channelid"), program,
                         epgData.get(index, column, "synopsis"), starttime, endtime, info,
                         recording, epgData.get(index, column, "recordid"), this)
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
                                     "info": epgData.get(i, c, "programid"),
                                     "recording": epgData.get(i, c, "recording")
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
            timeDelegate.createObject(timeFlick.contentItem, { "time": j })
        }
    }

    function showInfo(channelID, title, synopsis, starttime, endtime, info, recording, recordid, progDel) {
        infoPopup = infoPopupComp.createObject(layout, {"id": "myinfopopup", "channelID": channelID, "title": title, "starttime": starttime,
                               "endtime":endtime, "info":info, "synopsis": synopsis, "recording": recording,
                               "recordid": recordid, "progDel": progDel})
    }

    Component {
        id: infoPopupComp
        GenericPopup {
            z: 200
            property string channelID
            property string synopsis
            property real starttime
            property real endtime
            property string recording
            property string recordid
            property variant progDel
            anchors.fill: parent
            anchors.margins: 30
            content: Item {
                anchors.fill: parent
                anchors.margins: 20
                StyledText {
                    id: timeText
                    anchors {
                        right: parent.right
                        left: parent.left
                        top:parent.top
                    }
                    height : 50
                    text: getTimeString(starttime) + " - " + getTimeString(endtime)
                }
                StyledText {
                    id: synopsisText
                    anchors {
                        top: timeText.bottom
                        right: parent.right
                        left: parent.left
                        bottom: buttonRect.top
                    }

                    text: synopsis
                }
                Rectangle {
                    id: buttonRect
                    anchors {
                        right: parent.right
                        left: parent.left
                        bottom: parent.bottom
                    }
                    height : Style.appButtonHeight*1.5
                    border.color: "white"
                    border.width: 2
                    opacity: .5

                    Row {
                        anchors.fill: parent
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.margins: 20
                        StyledButton {
                            id: recordBt
                            label: recording == '' ? qsTr("Record") : qsTr("Cancel record")
                            onActivated: {
                                var obj = { 'progDel': progDel};
                                var currentCB;
                                if (recording == 'O') {
                                    currentCB = manager.cancelRecording(recordid, '')
                                    obj.type = 'CR'
                                } else {
                                    currentCB = manager.scheduleRecording('O', channelID+","+starttime+","+endtime)
                                    obj.type = 'SR'
                                }
                                console.log("sent DCE command, got cbno = " + currentCB)
                                callbacks[currentCB] = obj;
                            }
                        }
                    }
                }
            }
        }
    }

    // Dummy: we need this to trigger loading of the actual data until I figure out how to trigger it automatically
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

    Flickable {
        id: channelCol
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.topMargin: cellHeight
        width: 150
        z: 100
        contentY: programFlick.contentY
        contentHeight: 1200
        clip: true

    }

    Flickable {
        id: timeFlick
        anchors.top: parent.top
        anchors.left: channelCol.right
        anchors.right: parent.right
        height: cellHeight
        contentWidth: 1200
        contentHeight: cellHeight
        contentX: programFlick.contentX
        clip: true
        flickableDirection: Flickable.HorizontalFlick
    }

    Flickable {
        id: programFlick
        anchors.top: timeFlick.bottom
        anchors.left: channelCol.right
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        contentWidth: 1200
        contentHeight: contentItem.childrenRect.height
        clip: true
        flickableDirection: Flickable.HorizontalAndVerticalFlick
    }

}
