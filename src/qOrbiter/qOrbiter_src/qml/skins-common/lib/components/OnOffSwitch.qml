import QtQuick 1.1

Item {
    width: scaleX(30)
    height: scaleY(12)
    anchors.centerIn: parent

    Rectangle{
        id:phild
        color: "grey"
        opacity: .85
        border.color: "white"
        border.width: 1
    }

    Row{
        id:options
        height: parent.height
        width: parent.width
        spacing: scaleX(2)
        Item{
            height:scaleY(6)
            width:scaleX(10)

            Rectangle{
                anchors.fill: parent
                color:ms.pressed ? "yellow" : "green"
                border.color: "white"
                border.width: 1
                radius:5

                Behavior on color {
                    PropertyAnimation{
                        duration: 250
                    }
                }

                Text{
                    text:"On"
                    color:"white"
                    font.pointSize: 22
                    anchors.centerIn: parent
                }
                MouseArea{
                    id:ms
                    anchors.fill: parent
                       onReleased: {setParam("on");cmdEntry.sendCommand()}
                }
            }
        }
        Item{
            height:scaleY(6)
            width:scaleX(10)

            Rectangle{
                anchors.fill: parent
                color:ms2.pressed ? "yellow" : "red"
                border.color: "white"
                border.width: 1
                radius:5

                Behavior on color {
                    PropertyAnimation{
                        duration: 250
                    }
                }

                Text{
                    text:"Off"
                    color:"white"
                    font.pointSize: 22
                    anchors.centerIn: parent
                }
                MouseArea{
                    id:ms2
                    anchors.fill: parent
                    onReleased: {setParam("off");cmdEntry.sendCommand()}
                }
            }
        }
    }
}
