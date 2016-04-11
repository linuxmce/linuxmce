import QtQuick 1.1

Item{
    height: appH
    width: appW

    Rectangle{
        anchors.fill: parent
        color:"#848482"
    }

    Text {
        id: reloadLabel
        text: qsTr("Reloading")
    }

}
