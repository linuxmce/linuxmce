import QtQuick 1.1

Item{
    id:styledHdr
    property string labelText:"lorem Ipsum foo"
    property bool useClose:true
    signal closeSignal()
    anchors{
        top:parent.top
        left:parent.left
        right:parent.right
    }
    height: scaleY(10)
    Rectangle{
        id:hdrFill
        anchors.fill: parent
        color:appStyle.headerBgColor
        gradient:appStyle.bgHeaderGradient
    }

    StyledText{
        anchors.centerIn: parent
        text:labelText
    }

    StyledButton{
        anchors.right: parent.right
        visible:useClose
        buttonText:"Close"
        onActivated: {
        closeSignal()
        }
    }
}
