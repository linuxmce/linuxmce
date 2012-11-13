import QtQuick 1.0
import "../components"
Item{
    id:sleepingmenu

    Rectangle{
        height: manager.appHeight
        width: manager.appWidth
        color: style.darkhighlight
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
        Component.onCompleted: manager.getSleepingAlarms()
        Rectangle{
            id:mainsleepingrect
            height:scaleY(65)
            width: scaleX(80)
            color:style.lighthighlight
            anchors.centerIn: parent
            radius: 10
            clip:true
            border.color: "black"
            border.width: 3

            Row{
                id:containerrow
                spacing: scaleY(1)
                height: childrenRect.height
                width: childrenRect.width
                anchors.verticalCenter: parent.verticalCenter
                Image {
                    id: clockicon
                    source: "../img/icons/alarmclock.jpg"
                    height: scaleY(35)
                    fillMode: Image.PreserveAspectFit
                    width: scaleX(35)
                }

                Rectangle{
                    id:alarmslist
                    height: childrenRect.height
                    width: childrenRect.width
                    color:"transparent"
                    clip:true

                    Column{
                        id:alarmslayoutcolumn
                        spacing: scaleY(1)
                        width: childrenRect.width
                        Rectangle{
                            id:alarmsheader
                            height: scaleY(4)
                            width: scaleX(18)
                            color: style.accentcolor
                            radius: 5
                            Text {
                                id: alarmtext
                                text: qsTr("Sleeping Alarms")
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                        }

                        Rectangle{
                            height: childrenRect.height
                            width: scaleX(25.25)
                            color:style.lighthighlight

                            ListView{
                                id:alarmlist
                                height: scaleY(20)
                                width: scaleX(25)
                                model:alarms
                                anchors.centerIn: parent
                                anchors.margins: 10
                                delegate:
                                    Rectangle
                                {
                                    height: scaleY(10)
                                    width: scaleX(25)
                                    color:"white"
                                    border.color: "black"
                                    border.width: 2
                                    anchors.margins: 10
                                    Row{
                                        id:delegatalayoutrow
                                        anchors.centerIn: parent
                                        width: parent.width
                                        spacing: 5
                                        Rectangle{
                                            id:indicator
                                            height: scaleY(10)
                                            width: parent.width *.25
                                            color: b_state ?"green":  "red"
                                            border.color: "black"
                                            border.width: 2
                                            Component.onCompleted: console.log(b_state)
                                            Text {
                                                id: statelabe
                                                text: status
                                                anchors.centerIn: parent
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
                                            Text {
                                                id: alarmname
                                                text:qsTr("Name: ") + name
                                            }
                                            Text {
                                                id: alarmtimeLabel
                                                text:qsTr("Alarm Set For: ") + alarmtime
                                            }
                                            Text {
                                                id: daysactive
                                                text: qsTr("Active on: ")+ active
                                            }
                                            Text {
                                                id: countdown
                                                text: qsTr("Time Left: ") +remaining
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
