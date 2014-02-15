import QtQuick 1.0

Item{
    id:btnBase
    height: style.stdbuttonh
    width: style.stdbuttonw
    property double radius:5
    property alias buttontext: buttonLabel.text
    property alias buttontextcolor: buttonLabel.color
    property alias buttontextfontsize: buttonLabel.font.pixelSize
    property alias buttontextbold: buttonLabel.font.bold
    property alias buttontextitalic: buttonLabel.font.italic
    property alias buttontextzindex: buttonLabel.z
    property alias buttonsqradius:  buttonBase.radius
    property int textSize: style.buttonLabelText
    property color color: style.button_system_color
    property color textColor: style.button_system_text_color
    property string imgSource:""
    property color activatedColor:style.button_system_color_hover
    signal activated()

    Rectangle {
        id:buttonBase
        anchors.fill: btnBase
        color: ms.pressed ? activatedColor : btnBase.color
	border.width:1
	border.color: "white"
        radius: btnBase.radius
        opacity:.65
        smooth:true
    }

    Image {
        source: imgSource
        anchors.fill: parent
    }

    Text {
        id: buttonLabel
        text:"null ipsum delorium"
        font.pixelSize: textSize
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.fill: parent
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.margins: 5
        font.weight: Font.Light
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        color: textColor
    }

    MouseArea{
        id:ms
        anchors.fill: parent
        hoverEnabled: true
        onReleased:activated()
    }
}
