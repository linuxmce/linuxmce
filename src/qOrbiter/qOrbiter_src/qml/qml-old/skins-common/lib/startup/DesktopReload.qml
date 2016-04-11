import QtQuick 1.1

Item{
height:appH
width: appW

    Rectangle{
        anchors.fill: parent
        color:"#848482"
    }
    Text {
        text: qsTr("LINUXMCE")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        color:"white"
    }

    Text {
        id: reloadLabel
        text: qsTr("Reloading")
        anchors.centerIn: parent
        color:"white"
    }

}
