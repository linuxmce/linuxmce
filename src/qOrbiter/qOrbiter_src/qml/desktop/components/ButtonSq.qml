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

    color:style.lighthighlight
    height: style.stdbuttonw
    width: style.stdbuttonw
    border.width: 1
    border.color: style.button_action_color_hover
    radius: 1

    Text {
        id: buttonLabel
        x: 50
        y: 50
        width: -1
        height: 0
        text:"null ipsum delorium"
        font.pixelSize: 14
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.fill: parent
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.margins: 5
        font.family: "Droid Sans"
        wrapMode: Text.WordWrap
        z:1
}
        MouseArea{
            anchors.fill: buttonLabel
            hoverEnabled: true
            onEntered: {
                buttonBase.color = style.highlight2
                buttonLabel.color = style.lighthighlight
            }
            onExited: {
                buttonBase.color = style.lighthighlight
                buttonLabel.color = "Black"
            }


    }


}
