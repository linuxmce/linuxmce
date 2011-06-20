import QtQuick 1.0

Rectangle {
    id:buttonBase
     property alias buttontext: buttonLabel.text

    Text {
        id: buttonLabel
        text:""
        anchors.centerIn: parent
         }


    MouseArea
    {   anchors.fill: parent
        onClicked: console.log(buttonLabel.text + " Clicked. MessageSend that!")

    }
}
