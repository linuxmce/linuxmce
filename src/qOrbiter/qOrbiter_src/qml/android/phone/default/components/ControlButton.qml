import QtQuick 1.1
import "../components"

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
        gradient:Gradient {
                     GradientStop{ position:0.0;color:"transparent"}
                     GradientStop{ position:0.65;color:"black"}
                 }
        opacity: .65
    }

    
    Rectangle{
        id:fil
        anchors.fill: parent
        radius:parent.height
        color:ms.pressed ? "green": "black"
        opacity:.65
        Behavior on color{
            ColorAnimation {  duration: 200 }
        }

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
