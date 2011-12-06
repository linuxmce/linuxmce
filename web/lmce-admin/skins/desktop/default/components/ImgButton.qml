import QtQuick 1.0



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

    height: style.stdbuttonw
    width: style.stdbuttonw
    border.width: 3
    border.color: style.button_action_color_hover
    radius: 1
    Image {
        id: buttonimage
        source: "../../../img/icons/buttonoverlay.png"
        height: parent.height
        width: parent.width

    }
    Text {
        id: buttonLabel
        anchors.centerIn: parent
        text:"null ipsum delorium"
        font.pixelSize: 14
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.fill: parent
        anchors.margins: 5
        font.family: "Droid Sans"
        wrapMode: Text.WordWrap
       }



    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            buttonBase.border.color = "white"
            buttonimage.opacity = .15
        }
        onExited: {
            buttonBase.border.color = style.button_action_color_hover
            buttonimage.opacity = .75
        }
    }



}


