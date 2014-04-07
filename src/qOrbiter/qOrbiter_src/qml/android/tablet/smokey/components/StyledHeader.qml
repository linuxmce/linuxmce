import QtQuick 1.1
import "../../../../skins-common/lib/handlers"

Item{
    id:styledHdr
    property string labelText:"lorem Ipsum foo"
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
        color:style.headerBgColor
        gradient:style.bgHeaderGradient
    }

    StyledText{
        anchors.centerIn: parent
        text:labelText
    }

    StyledButton{
        anchors.right: parent.right
        label:"Close"
        onActivated: {
        closeSignal()
        }
    }
}
