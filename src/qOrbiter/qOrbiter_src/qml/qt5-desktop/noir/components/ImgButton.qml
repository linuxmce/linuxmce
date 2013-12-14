import QtQuick 2.0



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

    height: style.stdbuttonh
    width: style.stdbuttonw

    radius: 1
    Image {
        id: buttonimage
        source: ""
        height: style.stdbuttonh
        width: style.stdbuttonw

    }
    StyledText {
        id: buttonLabel
        anchors.centerIn: parent
        text:"null ipsum delorium"
        font.pointSize: 14
        verticalAlignment: StyledText.AlignVCenter
        horizontalAlignment: StyledText.AlignHCenter
        anchors.fill: parent
        anchors.margins: 5
        font.family: "Droid Sans"
        wrapMode: StyledText.WordWrap
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


