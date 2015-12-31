import QtQuick 1.0
Rectangle {
    id:buttonMain

    property alias textSize: buttonLabel.font.pointSize
    signal clicked
    property variant text

    color: "transparent"

    Rectangle{
            id:buttonBg
            width: parent.width
            opacity: 0.800
            height: parent.height
            color: "#000000"
    }

    Text {
        id: buttonLabel
        text:parent.text
        font.pointSize: 34
        color: "#ffffff"
        anchors.centerIn:parent
        font.family: "Krungthep"
        wrapMode: Text.WordWrap

         }

    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            buttonBg.opacity=0.500
        }
        onExited:  {
            buttonBg.opacity=0.800
        }
        onClicked: buttonBg.clicked()
    }

}
