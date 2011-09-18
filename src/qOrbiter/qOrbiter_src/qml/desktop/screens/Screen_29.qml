import QtQuick 1.0
import "../components"
Item{
    id:securitystatus

    Rectangle{
        height: style.orbiterH
        width: style.orbiterW
        color: style.darkhighlight
        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
        Component.onCompleted: sleepingMenu()
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
                    source: "../../../img/icons/alarmclock.jpg"
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
                            width: scaleX(15)
                            color: style.accentcolor
                            radius: 5
                            Text {
                                id: alarmtext
                                text: qsTr("Sleeping Alarms")
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                        }

                        Rectangle{
                            height: scaleY(10)
                            width: scaleX(15)
                            color:style.lighthighlight
ListView{
    id:alarmlist
    height: scaleY(9)
    width: scaleX(14)
    model:alarms
    delegate: Rectangle{
        height: scaleY(8)
        width: scaleX(13)
        Row{
            id:delegatalayoutrow
            anchors.centerIn: parent
            Rectangle{
                id:indicator
                height: scaleY(5)
                width: scaleX(5)
                color: state ? red : green
            }
            Column{

                Text {
                    id: alarmtime
                    text: alarmTime
                }
                Text {
                    id: daysactive
                    text: activedays
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
