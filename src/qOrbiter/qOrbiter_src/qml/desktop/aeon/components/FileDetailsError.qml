import QtQuick 1.1

Rectangle {
    id: details
    height: style.orbiterH
    width: style.orbiterW
    color: "transparent"
    Rectangle{
        anchors.fill: parent
        color: "#000000"
        opacity: 0.800
    }
    MouseArea {
        anchors.fill: parent

        Rectangle {
            anchors.centerIn: parent
            width: scaleX(35)
            height: scaleY(28)
            color: "#ffffff"

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                y: scaleY(7)
                text: "Error: No qml file found for media type "+gmediaType
                smooth: true
                font.family: aeonEdit.name
                font.pixelSize: scaleY(2.2)
            }

            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                y: scaleY(17) // 134
                width: scaleX(10)
                height: scaleY(6)
                color: "#ffffff"
                border.color: "black"

                Text {
                    anchors.centerIn: parent
                    text: "Ok"
                    font.pixelSize: scaleY(3)
                    smooth: true
                    font.family: aeonEdit.name
                }

                MouseArea {
                    id: mouse_area2
                    anchors.fill: parent
                    onClicked: {
                        details.destroy()
                    }
                }
            }
        }
    }
}
