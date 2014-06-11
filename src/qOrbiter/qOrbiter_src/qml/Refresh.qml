import QtQuick 1.1

Item {
    anchors.fill: parent
    Rectangle{
        anchors.fill: parent
        color:"black"
    }

    Text{
        anchors.centerIn: parent
        text:"Reloading Component Cache, please be patient"
        font.pointSize: 28
    }
}
