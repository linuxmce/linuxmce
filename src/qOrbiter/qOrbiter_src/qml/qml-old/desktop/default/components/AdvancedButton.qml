import QtQuick 1.1

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
        color: "transparent"
        height: parent.height
        width: parent.width

        Image {
            id: buttonbg
            source: "../img/ui3/linuxmcewidebutton.png"
            height: parent.height
            width: parent.width
            opacity: .5
        }

        StyledText {
            id: buttonLabel
            x: 50
            y: 50
            width: -1
            height: 0
            textLabel:"null ipsum delorium"
            fontSize: scaleY(2)
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
            anchors.fill: buttonLabel
            hoverEnabled: true
            onEntered: {
                buttonbg.opacity = 1
                buttonLabel.color = skinStyle.lighttext
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
