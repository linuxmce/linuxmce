import QtQuick 2.2


Item{
    id:buttonBase
    height: 100
    width: height
    property string buttonLabel: "lorem ipsum"
    property bool pressed:ms.pressed
    signal activated()
    
    Rectangle{
        anchors.fill: parent
        radius: parent.height
        gradient:style.buttonGradient
        opacity: .65
    }

    
    Rectangle{
        id:fil
        anchors.fill: parent
        radius:parent.height
        color:ms.pressed ? style.buttonPressedColor: "black"
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
