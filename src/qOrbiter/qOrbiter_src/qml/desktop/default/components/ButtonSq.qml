import QtQuick 1.1

Rectangle{

    color: "transparent"
    height: skinStyle.stdbuttonw
    width: skinStyle.stdbuttonw
    property alias buttontext: buttonLabel.text
    property alias buttontextcolor: buttonLabel.color
    property alias buttontextfontsize: buttonLabel.fontSize
    property alias buttontextbold: buttonLabel.font.bold
    property alias buttontextitalic: buttonLabel.font.italic
    property alias buttontextzindex: buttonLabel.z
    property alias buttonsqradius:  buttonBase.radius
    property alias buttonbackground: buttonImage.source
    property alias clickHandler:buttonHandler


    Rectangle {
        id:buttonBase
        color:skinStyle.button_system_color
        height: parent.height
        width: parent.width
        border.width: 2
        border.color: skinStyle.highlight1
        radius: 5

    Image {
        anchors.fill: parent
        id: buttonImage
        x: 0
            y: 0
        height: parent.height
        width: parent.width
    }

        StyledText {
            id: buttonLabel
            x: 50
            y: 50
            width: -1
            height: 0
            textLabel:"null ipsum delorium"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            anchors.fill: parent
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 5

            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            // elide: "ElideRight"
            color:"black"
            z:1
        }
        MouseArea{
            id:buttonHandler
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
