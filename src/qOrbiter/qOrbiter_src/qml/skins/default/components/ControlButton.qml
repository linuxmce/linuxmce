import QtQuick 2.2
import "../components"
import "../."
Item{
    id:buttonBase
    height: 100
    width: height
    property string buttonLabel: "lorem ipsum"
    property bool pressed:ms.pressed
    signal activated()

    
    Rectangle{
        id:fil
        anchors.fill: parent
        radius:parent.height
        color:ms.pressed ? appStyle.appbutton_confirm_color: "black"
        opacity:.35

    }
    
    StyledText{
        text: buttonLabel
        anchors.centerIn: parent        
    }
    
    MouseArea{
        id:ms
        anchors.fill: parent
        onReleased:activated()
    }
}
