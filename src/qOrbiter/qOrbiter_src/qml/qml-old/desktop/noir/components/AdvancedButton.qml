import QtQuick 1.1
    Rectangle {

        id:buttonBase
        color: "black"
        property alias buttoncolor: buttonBase.color

        height: style.stdbuttonw
        width: style.stdbuttonw
        property alias buttontext: buttonLabel.text
        property alias buttontextcolor: buttonLabel.color

        property alias buttontextbold: buttonLabel.font.bold
        property alias buttontextitalic: buttonLabel.font.italic
        property alias buttontextzindex: buttonLabel.z
        property alias buttonsqradius:  buttonBase.radius

        /*
        Image {
            id: buttonbg
            source: "../img/ui3/linuxmcewidebutton.png"
            height: parent.height
            width: parent.width
            opacity: .5
        }
*/
        Text {
            id: buttonLabel
            x: 50
            y: 50
            width: -1
            height: 0
            text:"null ipsum delorium"
            font.pixelSize: 14
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            anchors.fill: parent
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 5
            font.family: "Droid Sans"
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            // elide: "ElideRight"
            color:"White"
            z:1
        }
        MouseArea{
            anchors.fill: buttonLabel
            hoverEnabled: true
            onEntered: {
                buttonLabel.font.capitalization = Font.AllUppercase
            }

            onExited: {
                buttonLabel.font.capitalization = Font.Normal               
            }
        }
    }

