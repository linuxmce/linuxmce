import QtQuick 1.1

Rectangle{

    color: style.homescreenscenariobuttoncolor
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
        color:style.button_system_color
        height: parent.height
        width: parent.width
        border.width: 2
        border.color: style.highlight1
        radius: 5

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
            color:"black"
            z:1
        }
        MouseArea{
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
}
