import QtQuick 1.0

Rectangle{

    color: "blue"
    height: style.stdbuttonh
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
        color: "blue"
        height: parent.height
        width: parent.width

        Image {
            id: buttonbg
            source: "../img/buttonbg.png"
            height: parent.height
            width: parent.width
            opacity: .5
        }

        Text {
            id: buttonLabel
            width: -1
            height: 0
            text:"null ipsum delorium"
            font.pixelSize: scaleY(3)
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
            z:1
        }
        MouseArea{
            anchors.fill: buttonLabel
            hoverEnabled: true
            onEntered: {
               buttonbg.opacity = 1
                buttonLabel.color = style.titlecolor
                buttonLabel.font.capitalization = Font.AllUppercase
            }

            onExited: {
             buttonbg.opacity = .85
                buttonLabel.font.capitalization = Font.Normal
                buttonLabel.color = "black"
            }
        }
    }
}
