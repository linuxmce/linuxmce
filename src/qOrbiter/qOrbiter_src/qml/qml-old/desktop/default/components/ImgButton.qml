import QtQuick 1.1



Rectangle {
    id:buttonBase
    property alias buttontext: buttonLabel.text
    property alias buttontextcolor: buttonLabel.color
    property alias buttontextfontsize: buttonLabel.font.pointSize
    property alias buttontextbold: buttonLabel.font.bold
    property alias buttontextitalic: buttonLabel.font.italic
    property alias buttontextzindex: buttonLabel.z
    property alias buttonsqradius:  buttonBase.radius
    property alias buttonbordercolor: buttonBase.border
    property alias buttonopacity: buttonimage.opacity

    height: skinstyle.stdbuttonh
    width: skinstyle.stdbuttonw

    radius: 1
    Image {
        id: buttonimage
        source: ""
        anchors.fill: parent

    }
    StyledText {
        id: buttonLabel
        anchors.centerIn: parent
        textLabel:"null ipsum delorium"
        fontSize: 14
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        width: parent.width
        anchors.margins: 10
        font.bold: false
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
       }

    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {

            buttonimage.opacity = .5
        }
        onExited: {

            buttonimage.opacity = 1
        }
    }

}


