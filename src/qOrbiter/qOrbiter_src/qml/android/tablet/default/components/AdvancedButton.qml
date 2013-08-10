import QtQuick 1.0

Item{
    id:advancedRoot
    property string color: "transparent"
    height: style.stdbuttonh
    width: style.stdbuttonw
    property alias buttontext: buttonLabel.text
    property alias buttontextcolor: buttonLabel.color
    property alias buttontextfontsize: buttonLabel.font.pointSize
    property alias buttontextbold: buttonLabel.font.bold
    property alias buttontextitalic: buttonLabel.font.italic
    property alias buttontextzindex: buttonLabel.z
    property alias buttonsqradius:  buttonBase.radius
    property string imgSource:""
    signal btnPress()
    Rectangle {
        id:buttonBase
        color: ms.pressed ? "white" : "darkblue"
        height: parent.height
        width: parent.width
        opacity:ms.pressed ? .65 : .45
        border.color: "white"
        border.width: 2
        radius:10
        Behavior on color {
            PropertyAnimation{
                duration:500
            }
        }

        Behavior on opacity {
            PropertyAnimation{
                duration:500
            }
        }


    }
    Image {
        id: buttonbg
        source:imgSource
        height: parent.height
        width: parent.width
        opacity: .5
    }

    StyledText {
        id: buttonLabel
        text:"null ipsum delorium"
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.fill: parent
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.margins: 5
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        color:"White"

    }

    MouseArea{
        id:ms
        anchors.fill: advancedRoot
        hoverEnabled: true
        onEntered: {
            buttonLabel.color = style.lighttext
            buttonLabel.font.capitalization = Font.AllUppercase
        }

        onExited: {
            buttonLabel.font.capitalization = Font.Normal
            buttonLabel.color = "white"
        }
        onPressed: btnPress()
    }
}
