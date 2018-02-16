import QtQuick 2.2
import "../components"
import "../."
StyledScreen {
    anchors.fill: parent
    screen: qsTr("Router Reloading")



    Panel{
        useHeader: false
        anchors.centerIn: parent

        StyledText{
            anchors.centerIn: parent
            text:"The LinuxMCE Core is reloading. Please be patient and it will retart in a moment"
            width: parent.width
            fontSize: appStyle.appFontSize_title
        }
    }



}
