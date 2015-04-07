// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id: rectangle2
    width: 800
    height: 800

    Rectangle {
        id: rectangle1
        x: 0
        y: 449
        width: 500
        height: 51
        color: "#f50000"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0

    }

    Rectangle {
        id: rectangle3
        x: 136
        y: 339
        width: 200
        height: 200
        color: "#ffffff"
        Text {
        id: text1
        x: 102
        y: 57
        text: qsTr("text")
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.horizontalCenter: rectangle1.horizontalCenter
        font.pixelSize: 12
        }
    }


}
