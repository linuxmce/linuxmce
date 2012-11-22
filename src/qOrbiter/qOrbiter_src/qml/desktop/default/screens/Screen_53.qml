import QtQuick 1.0
import "../components"
Rectangle {
    height: appH
    width: appW
    color: "transparent"
    Rectangle{
        anchors.fill: parent
        color: style.warnColor
        opacity: .25
    }
    StyledText{
        text: qsTr("I finished installing the software for your new devices. You will need to restart the router before you can use them.")
        font.pointSize: 18
        anchors.centerIn: parent
        font.bold: true
    }
    HomeButton{
        id:home
    }

}
