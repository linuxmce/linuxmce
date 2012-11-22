import QtQuick 1.0

Rectangle {
    id: btHomeDefault
    height: style.btHomeDefaultH
    width: style.btHomeDefaultW
    radius: style.but_smooth
    color:"transparent"

    property string currentLink: "submenu"
    property string currentImage: "../images/btTelecom.png"
    property string currentText: "Telecom"
    signal clicked

    Rectangle {
        id: bgColor
        height: parent.height
        width: parent.width
        radius: parent.radius
        color:"#000000"
        opacity: 0.800
    }

    Image {
        id: buttonbg
        source: btHomeDefault.currentImage
        height: Image.height
        width: Image.width
        anchors.top: parent.top
        anchors.topMargin: scaleX(1.5)
        anchors.horizontalCenter: btHomeDefault.horizontalCenter
        opacity: 1
    }
    Text{
        id: text
        text: btHomeDefault.currentText
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.horizontalCenter: btHomeDefault.horizontalCenter
        font.pixelSize: 18
        color: "#ffffff"
        opacity: 1
    }
    Image {
        id: buttonbg2
        source: "../images/arrowDown.png"
        visible: false
        height: Image.height
        width: Image.width
        anchors.bottom: parent.top
        anchors.horizontalCenter: btHomeDefault.horizontalCenter
    }


    MouseArea{
        id: mousearea2
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            bgColor.opacity=0.500
            buttonbg2.visible=true
        }

        onClicked: {
            //manager.setFloorplanType(6)
            //manager.ShowFloorPlan(6)
        }
        onExited: {
            bgColor.opacity=0.800
            buttonbg2.visible=false
        }
    }
}
