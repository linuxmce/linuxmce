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

    height: skinStyle.stdbuttonh
    width: skinStyle.stdbuttonw
    clip:true
    radius: 1
    Image {
        id: buttonimage
        source: ""
        height: skinStyle.stdbuttonh
        width: skinStyle.stdbuttonw

    }
    StyledText {
        id: buttonLabel
        text:"null ipsum delorium"
        font.pixelSize: 14

        font.family: keyFont.name
        wrapMode: Text.WordWrap
        anchors.centerIn: buttonimage
        anchors.verticalCenterOffset: -15
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


