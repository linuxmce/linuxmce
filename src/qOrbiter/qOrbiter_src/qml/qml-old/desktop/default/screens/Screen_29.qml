import QtQuick 1.1
import "../components"
import "../../../skins-common/lib/handlers"
import org.linuxmce.enums 1.0
Item{
    id:sleepingmenu

    Rectangle{
        height: manager.appHeight
        width: manager.appWidth
        Component.onCompleted: {
        }

        color: "transparent"
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
        Rectangle{
            id:mainsleepingrect
            height:scaleY(65)
            width: scaleX(80)
            color:skinstyle.lighthighlight
            anchors.centerIn: parent
            radius: 10
            clip:true
            border.color: "black"
            border.width: 3
            StyledText {
                id: alarmtext
                text: qsTr("Sleeping Alarms")
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                style: Text.Sunken
                font.pointSize: scaleY(4)
            }
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
                            height: childrenRect.height
                            width: scaleX(25.25)
                            color:"transparent"
                            clip:true

                            ListView{
                                id:alarmlist
                                height: scaleY(35)
                                width: scaleX(25)
                                model: manager.getDataGridModel("sleepingAlarms", DataGrids.Alarms_In_Room, String(manager.getlocation()))
                                anchors.centerIn: parent
                                anchors.margins: 10

                                delegate:
                                    Rectangle
                                {
                                height: scaleY(15)
                                width: scaleX(25)
                                color:skinstyle.darkhighlight
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
                                        height: scaleY(15)
                                        width: parent.width *.25
                                        color: status ?"green":  "red"
                                        border.color: "black"
                                        border.width: 2

                                        StyledText {
                                            id: statelabel
                                            text: status === true ? qsTr("enabled") : qsTr("disabled")
                                            anchors.centerIn: parent
                                        }

                                        AlarmToggleHandler{
                                        }
                                    }
                                    Column{
                                        height: parent.height
                                        StyledText {
                                            id: alarmname
                                            text:qsTr("Name: ") + name
                                        }
                                        StyledText {
                                            id: alarmtimeLabel
                                            text:qsTr("Alarm Set For: ") + alarmtime
                                        }
                                        StyledText {
                                            id: daysactive
                                            text: qsTr("Active on: ")+ active
                                        }
                                        StyledText {
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
