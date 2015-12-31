import QtQuick 1.1
import "../components"
import "../../../skins-common/lib/handlers"
Item{
    id:sleepingmenu

    Rectangle{
        height: appH
        width: appW
        color: style.darkhighlight
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
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
                                model: manager.getDataGridModel("sleepingAlarms", 29)
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
                                                id: handler
                                                text: status
                                                anchors.centerIn: parent
                                            }

                                            AlarmToggleHandler{
                                            }
                                        }
                                        Column{
                                            spacing: 5
                                            Text {
                                                id: alarmname
                                                text:qsTr("Name: ") + name
                                            }
                                            Text {
                                                id: alarmtime
                                                text:qsTr("Alarm Set For: ") + alarmTime
                                            }
                                            Text {
                                                id: daysactive
                                                text: qsTr("Active on: ")+ activeDays
                                            }
                                            Text {
                                                id: countdown
                                                text: qsTr("Time Left: ") +timeLeft
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
