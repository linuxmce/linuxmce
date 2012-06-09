import QtQuick 1.0

Rectangle{

    color: "transparent"
    height: style.stdbuttonh
    width: style.stdbuttonw
    property alias buttontext: buttonLabel.text
    property alias buttontextcolor: buttonLabel.color
    property alias buttontextfontsize: buttonLabel.font.pixelSize
    property alias buttontextbold: buttonLabel.font.bold
    property alias buttontextitalic: buttonLabel.font.italic
    property alias buttontextzindex: buttonLabel.z
    property alias buttonsqradius:  buttonBase.radius
    property int textSize: style.buttonLabelText



    Rectangle {
        id:buttonBase

        height: style.stdbuttonh
        width: style.stdbuttonw
        color:"transparent"
        radius: 2.5
        smooth:true
        Image {

            source: "../img/buttonbg.png"
            anchors.fill: parent

        }
        Text {
            id: buttonLabel
            text:"null ipsum delorium"
            font.pixelSize: textSize
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
