import QtQuick 1.0

Rectangle {
    id:buttonBase
     property alias buttontext: buttonLabel.text

    Text {
        id: buttonLabel
        x: 51
        y: 50
        width: -1
        height: 0
        text:"null ipsum delorium"
        font.pointSize: 11
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.verticalCenterOffset: 0
        anchors.horizontalCenterOffset: 1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        font.family: "Bitstream Charter"
        wrapMode: Text.WordWrap
         }


    MouseArea
    {   anchors.fill: parent
        onClicked: console.log(buttonLabel.text + " Clicked. MessageSend that!")

    }
}
