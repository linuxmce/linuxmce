import QtQuick 1.1
import "../components"

Item{
    id:buttonBase
    height: 100
    width: height
    property string buttonLabel: "lorem ipsum"
    signal activated()
    
    Rectangle{
        anchors.fill: parent
        radius: parent.height
        gradient:style.buttonGradient
    }

    
    Rectangle{
        id:fil
        anchors.fill: parent
        radius:parent.height
        color:ms.pressed ? style.buttonPressedColor: style.buttonColor
        opacity:style.buttonOpacity

    }
    
    StyledText{
        text: buttonLabel
        anchors.centerIn: parent
        isBold: true
    }
    
    MouseArea{
        id:ms
        anchors.fill: parent
        onReleased:activated()
    }
}
