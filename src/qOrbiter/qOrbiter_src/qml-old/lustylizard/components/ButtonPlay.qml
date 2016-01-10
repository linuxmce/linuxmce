import QtQuick 1.0

Rectangle{

    color: "transparent"
    height: style.stdbuttonw
    width: style.stdbuttonw
    property alias buttontext: buttonLabel2.text
    property alias buttontextcolor: buttonLabel2.color
    property alias buttontextfontsize: buttonLabel2.font.pointSize
    property alias buttontextbold: buttonLabel2.font.bold
    property alias buttontextitalic: buttonLabel2.font.italic
    property alias buttontextzindex: buttonLabel2.z
    property alias buttonsqradius:  buttonBase2.radius



    Rectangle {
        id:buttonBase2
        color:"transparent"
        height: parent.height
        width: parent.width
        Text {
            id: buttonLabel2
            x: 50
            y: 50
            width: -1
            height: 0
            text:"null ipsum delorium"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            anchors.leftMargin: scaleX(2)
            anchors.fill: parent
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            font.family: "Droid Sans"
            font.pointSize: 20
            opacity: 1
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            // elide: "ElideRight"
            color:"#ffffff"
            z:1
        }
        MouseArea{
            anchors.fill: buttonBase2
            hoverEnabled: true
            onEntered: {
                buttonBase2.color = "#000000"
                buttonBase2.opacity = 1
                buttonLabel2.color = "#ffffff"
                buttonLabel2.opacity = 1
            }

            onExited: {
                buttonBase2.color = "#000000"
                buttonBase2.opacity = 0.400
                buttonLabel2.color = "#ffffff"
            }
        }
    }
}
