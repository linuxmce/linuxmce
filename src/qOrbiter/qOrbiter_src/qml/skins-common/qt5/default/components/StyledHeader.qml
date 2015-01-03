import QtQuick 2.2
import "../../../../skins-common/lib/handlers"

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
        color:style.headerBgColor
        opacity: style.bgLightOpacity
    }

    StyledText{
        anchors.centerIn: parent
        text:labelText
        color: "white"
    }

    StyledButton{
        anchors.right: parent.right
        visible:useClose
        label:"Close"
        onActivated: {
            closeSignal()
        }
    }
}
