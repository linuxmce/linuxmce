import QtQuick 1.0

Rectangle{

    color: "transparent"
    height: style.stdbuttonw
    width: style.stdbuttonw
    property alias buttontext: buttonLabel.text
    property alias buttontextcolor: buttonLabel.color
    property alias buttontextfontsize: buttonLabel.font.pointSize
    property alias buttontextbold: buttonLabel.font.bold
    property alias buttontextitalic: buttonLabel.font.italic
    property alias buttontextzindex: buttonLabel.z
    property alias buttonsqradius:  buttonBase.radius



    Rectangle {
        id:buttonBase

        height: parent.height
        width: parent.width
        border.width: 2
        border.color: "aliceblue"
        radius: 5
        Image {

            source: "../img/buttonbg.png"
            anchors.fill: parent

        }
        Text {
            id: buttonLabel
            x: 50
            y: 50
            width: -1
            height: 0
            text:"null ipsum delorium"
            font.pixelSize: 10
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            anchors.fill: parent
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 5
            font.family: "Droid Sans"
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            // elide: "ElideRight"
            color:"black"

        }
        MouseArea{
            anchors.fill: buttonLabel           


            onExited: {

                buttonLabel.font.capitalization = Font.Normal
                buttonLabel.color = "black"
            }
        }
    }
}
