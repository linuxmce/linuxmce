import QtQuick 1.0
import "../components"
Item{
    id:sleepingmenu
    height: manager.appHeight
    width: manager.appWidth

    Component.onCompleted: manager.getSleepingAlarms()

    Rectangle{
        id:phil
        anchors.fill:parent
        color:"darkred"
        opacity: .35
        border.width: 1
        border.color: "white"
    }

    Image {
        id: clockicon
        source: "../img/icons/alarmclock.jpg"
        height: scaleY(35)
        fillMode: Image.PreserveAspectFit
        width: scaleX(35)
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
    }

    ListView{
        id:alarmlist
        height: (count+1) * scaleY(25)
        width: scaleX(65)
        model:alarms
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: clockicon.right
        anchors.margins: 10
        delegate:
            Item{
            height: scaleY(35)
            width: scaleX(60)
            clip:true
            Rectangle{
                color:"darkblue"
                border.color: "white"
                border.width: 2
                opacity:.65
                anchors.fill: parent
            }

            Row{
                id:delegatalayoutrow
                anchors.centerIn: parent
                width: parent.width
                spacing: 5
                Rectangle{
                    id:indicator
                    height: scaleY(15)
                    width: parent.width *.25
                    color: status ? "green":  "red"
                    border.color: "black"
                    border.width: 2
                    opacity:.65

                    StyledText {
                        id: statelabel
                        text: status ? qsTr("Active") :  qsTr("Inactive")
                        anchors.centerIn: parent
                        color:"white"
                    }

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            manager.updateAlarm(!status, handler)
                        }
                    }
                }
                Column{
                    spacing: 5
                    StyledText {
                        id: alarmname
                        text:qsTr("Name: ") + name
                         color:"white"
                         fontSize: listViewText
                    }
                    StyledText {
                        id: alarmtimeLabel
                        text:qsTr("Alarm Set For: ") + alarmtime
                         color:"white"
                         fontSize: scaleY(5)
                    }
                    StyledText {
                        id: daysactive
                        text: qsTr("Active on: ")+ active
                         color:"white"
                         fontSize: scaleY(5)
                    }
                    StyledText {
                        id: countdown
                        text: qsTr("Time Left: ") +remaining
                         color:"white"
                         fontSize: scaleY(5)
                    }
                }
            }
        }
    }
     HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
}
