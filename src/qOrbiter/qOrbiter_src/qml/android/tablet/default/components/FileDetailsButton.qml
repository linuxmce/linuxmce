import QtQuick 1.0

Rectangle{
    id:detailButton
    color: "transparent"
    height: style.avoptionbuttonh
    width: scaleX(30)
    property alias buttontext: buttonLabel.text
    property alias buttontextcolor: buttonLabel.color
    property alias buttontextfontsize: buttonLabel.font.pointSize
    property alias buttontextbold: buttonLabel.font.bold
    property alias buttontextitalic: buttonLabel.font.italic
    property alias buttontextzindex: buttonLabel.z
    property alias buttonsqradius:  detailButton.radius

//    BorderImage {
//        id: name
//        verticalTileMode: BorderImage.Round
//        horizontalTileMode: BorderImage.Repeat
//        source: "../img/icons/drpshadow.png"
//        anchors.fill: parent
//        anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
//        border { left: 10; top: 10; right: 10; bottom: 10 }
//        smooth: true
//    }

    Rectangle{
        id:fill
        anchors.fill: parent
        color: "black"
        opacity: .75
    }


    StyledText {
        id: buttonLabel
        width: -1
        height: 0
        text:"null ipsum delorium"
        font.pixelSize: scaleY(5)
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.fill: parent
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.margins: 5
        isBold: true
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        // elide: "ElideRight"
        color:"orange"

    }
    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
         //   buttonBase.color = style.button_system_color_hover
            buttonLabel.color = style.lighttext
            buttonLabel.font.capitalization = Font.AllUppercase
        }

        onExited: {
          //  buttonBase.color = style.button_system_color
            buttonLabel.font.capitalization = Font.Normal
            buttonLabel.color = "black"
        }
    }

}
