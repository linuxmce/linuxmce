import QtQuick 1.0

Item{
    height: style.stdbuttonw
    width: style.stdbuttonw
    property alias buttontext: buttonLabel.text
    property alias buttontextcolor: buttonLabel.color
    property alias buttontextfontsize: buttonLabel.font.pointSize
    property alias buttontextbold: buttonLabel.font.bold
    property alias buttontextitalic: buttonLabel.font.italic
    property alias buttontextzindex: buttonLabel.z
    property alias buttonsqradius:  buttonBase.radius
    clip:true

    Rectangle {
        id:buttonBase
        color:ms.pressed ? "white": "darkblue"
        border.width: 2
        border.color: "white"
        radius: 5
        opacity: ms.pressed ? 1 : .35
        anchors.fill: parent

        Behavior on opacity {
            PropertyAnimation{
                duration:500
            }
        }

        Behavior on color{
            PropertyAnimation{
                duration: 500
            }
        }
    }

        Text {
            id: buttonLabel
            text:"null ipsum delorium"
            font.pixelSize: scaleY(5)
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter        
            anchors.margins: 5           
            font.bold: false
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            // elide: "ElideRight"
            color:"white"
        }
        MouseArea{
            id:ms
            anchors.fill: buttonLabel
            hoverEnabled: true
            onEntered: {
                buttonBase.color = style.button_system_color_hover
                buttonLabel.color = style.lighttext
                buttonLabel.font.capitalization = Font.AllUppercase
            }

            onExited: {
                buttonBase.color = style.button_system_color
                buttonLabel.font.capitalization = Font.Normal
                buttonLabel.color = "black"
            }
        }
    }

