import QtQuick 1.0

Rectangle {
    id:buttonBase
     property alias buttontext: buttonLabel.text
     property alias buttontextcolor: buttonLabel.color
     property alias buttontextfontsize: buttonLabel.font.pointSize
     property alias buttontextbold: buttonLabel.font.bold
     property alias buttontextitalic: buttonLabel.font.italic
     property alias buttontextzindex: buttonLabel.z



        height: 35
        width: 35
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

}
