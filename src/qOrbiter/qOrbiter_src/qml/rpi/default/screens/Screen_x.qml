import QtQuick 2.2

Item {
    width: scaleX(100)
    height: scaleY(100)

    Text {
        id: name
        text: qsTr("Page Not Found")
        font.pointSize: 32
        anchors.centerIn: parent
    }
}
