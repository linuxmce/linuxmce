import QtQuick 1.1

Item{
    anchors.fill: parent

    Rectangle{
        anchors.fill: parent
        color:"#848482"
    }

    Text {
        id: reloadLabel
        text: qsTr("Reloading")
    }

}
