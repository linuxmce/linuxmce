import QtQuick 1.0

Rectangle{

    color: "transparent"
    height: appStyle.stdbuttonh
    width: appStyle.stdbuttonw
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
        source: "../img/drpshadow.png"
        anchors.fill: buttonBase
        anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
        border { left: 10; top: 10; right: 10; bottom: 10 }
        smooth: true

    }


    Rectangle {
        id:buttonBase
        color:"transparent"
        height: appStyle.stdbuttonh
        width: appStyle.stdbuttonw
        Image {
            id: button_bg
            source: "../img/buttonbg.png"
            anchors.fill: parent
        }

        StyledText {
            id: buttonLabel
            width: -1
            height: 0
            text:"null ipsum delorium"
            font.pixelSize: scaleY(2)
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
        Image {
            id: hoverstate
            source: "../img/buttonhover.png"
            anchors.fill: parent
            visible:false

        }
        MouseArea{
            anchors.fill: buttonLabel
            onPressed: hoverstate.visible = true
            onReleased: hoverstate.visible = false
        }
    }
}
