import QtQuick 2.0

Rectangle{

    color: skinStyle.homescreenscenariobuttoncolor
    height: skinStyle.stdbuttonw
    width: skinStyle.stdbuttonw
    property alias buttontext: buttonLabel.text
    property alias buttontextcolor: buttonLabel.color
    property alias buttontextfontsize: buttonLabel.font.pointSize
    property alias buttontextbold: buttonLabel.font.bold
    property alias buttontextitalic: buttonLabel.font.italic
    property alias buttontextzindex: buttonLabel.z
    property alias buttonsqradius:  buttonBase.radius



    Rectangle {
        id:buttonBase
        color:skinStyle.button_system_color
        height: parent.height
        width: parent.width
        border.width: 2
        border.color: skinStyle.highlight1
        radius: 5

        StyledText {
            id: buttonLabel
            x: 50
            y: 50
            width: -1
            height: 0
            text:"null ipsum delorium"
            font.pointSize: 14
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
                buttonBase.color = skinStyle.button_system_color_hover
                buttonLabel.color = skinStyle.lighttext
                buttonLabel.font.capitalization = Font.AllUppercase
            }

            onExited: {
                buttonBase.color = skinStyle.button_system_color
                buttonLabel.font.capitalization = Font.Normal
                buttonLabel.color = "black"
            }
        }
    }
}
