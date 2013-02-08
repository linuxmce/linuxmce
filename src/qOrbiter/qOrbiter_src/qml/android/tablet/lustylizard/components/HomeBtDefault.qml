import QtQuick 1.0

Rectangle {
    id: btHomeDefault
    height: style.btHomeDefaultH
    width: style.btHomeDefaultW
    radius: style.but_smooth
    color:"#000000"
    opacity: 0.800

    property string currentImage: "../images/btTelecom.png"
    property string currentText: "Telecom"
    signal clicked

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
    MouseArea{
        id: mousearea1
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            btHomeDefault.opacity=0.500
        }
        onExited:  {
            btHomeDefault.opacity=0.800
        }
        onClicked: {
            manager.setFloorplanType(6)
            manager.ShowFloorPlan(6)
        }
    }
}
