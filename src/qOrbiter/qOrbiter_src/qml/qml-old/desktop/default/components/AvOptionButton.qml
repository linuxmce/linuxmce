import QtQuick 1.1

Rectangle{

    color: "transparent"
    height: skinstyle.avoptionbuttonh
    width: skinstyle.avoptionbuttonw
    property alias buttontext: buttonLabel.text
    property alias buttontextcolor: buttonLabel.color
    property alias buttontextfontsize: buttonLabel.font.pointSize
    property alias buttontextbold: buttonLabel.font.bold
    property alias buttontextitalic: buttonLabel.font.italic
    property alias buttontextzindex: buttonLabel.z
    property alias buttonsqradius:  buttonBase.radius

    BorderImage {
        id: name
verticalTileMode: BorderImage.Round
horizontalTileMode: BorderImage.Repeat
        source: "../img/icons/drpshadow.png"
        anchors.fill: buttonBase
        anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
        border { left: 10; top: 10; right: 10; bottom: 10 }
        smooth: true

    }

    Rectangle {
        id:buttonBase
        color:skinstyle.darkhighlight
        height: parent.height
        width: parent.width
        border.width: 2
        border.color: skinstyle.highlight1
        radius: 1


    }
    StyledText {
        id: buttonLabel
        width: -1
        height: 0
        textLabel:"null ipsum delorium"
        fontSize: scaleY(1.5)
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
        id:hitArea
        anchors.fill: buttonLabel
        hoverEnabled: true
        onEntered: {
            buttonBase.color = skinstyle.lighthighlight
            buttonLabel.color = skinstyle.lighttext
            buttonLabel.font.capitalization = Font.AllUppercase
        }

        onExited: {
            buttonBase.color = skinstyle.darkhighlight
            buttonLabel.font.capitalization = Font.Normal
            buttonLabel.color = "black"
        }
    }
}
