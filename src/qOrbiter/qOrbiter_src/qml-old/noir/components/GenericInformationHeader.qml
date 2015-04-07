// import QtQuick 2.0 // to target S60 5th Edition or Maemo 5
import QtQuick 2.0

Rectangle{
    id:header_row
    width: parent.width
    height: scaleY(10)
    color: "darkgray"

    StyledText{
        id:connectstatus
        text: qsTr("Orbiter")+ iPK_Device + qsTr(" Is Connected")
        color: "aliceblue"
        font.letterSpacing: 2
        anchors.left: parent.left
        anchors.leftMargin: scaleX(5)
        anchors.verticalCenter: header_row.verticalCenter
    }

    Clock{
        id:screen1time
        anchors.right: header_row.right
        anchors.rightMargin: scaleX(25)
        anchors.verticalCenter: header_row.verticalCenter
    }
}
