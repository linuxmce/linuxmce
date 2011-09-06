import QtQuick 1.0

Rectangle {
    id:powerrect
    width: 500
    height: 400
    clip: true
    color: "transparent"

    Image {
        id: bg
        fillMode: Image.PreserveAspectCrop
        source: "../../../img/icons/power.png"
        anchors.fill: parent

        Rectangle {
            id: on
            x: 44
            y: 68
            width: 413
            height: 51
            color: "transparent"

            Rectangle{
                color: "lightsteelblue"
                opacity: .5
                anchors.fill: parent
            }

            Text {
                id: onlabel
                text: qsTr("Display On")
                font.pointSize: 20
                font.family: "DroidSans"

                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: parent.font.bold = true
                    onClicked: powerrect.destroy()
                }

            }

            Text {
                id: mdonlabel
                text: qsTr("Media Director On")
              anchors.right: parent.right
                font.pointSize: 20
                font.family: "DroidSans"
            }

        }

        Rectangle {
            id: off
            x: 54
            y: 289
            width: 413
            height: 43
            color: "transparent"


            Rectangle{
                color: "lightsteelblue"
                opacity: .5
                anchors.fill: parent
            }

            Text {
                id: offlabel
                text: qsTr("Display Off")
                font.pointSize: 20
                font.family: "DroidSans"
            }

            Text {
                id: mdofflabel
                text: qsTr("Media Director Off")
              anchors.right: parent.right
                font.pointSize: 20
                font.family: "DroidSans"
            }

        }
    }
}
