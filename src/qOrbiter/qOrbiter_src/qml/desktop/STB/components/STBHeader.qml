import QtQuick 1.1
import Qt.labs.shaders 1.0
import "../../../skins-common/lib/components"

Item{
    id:hdr
    anchors{
        left:qmlroot.left
        right:qmlroot.right
    }
    height:scaleY(8)
    state:"hidden"
    property bool isActive: activeFocus
    Rectangle{
        anchors.fill: parent
        color:appStyle.primaryLightColor
        border.width: hdr.activeFocus ? 2 : 0
        focus:false
        border.color:"red"
        opacity: hdr.activeFocus ? 1 : .65
    }

    Row{
        anchors{
            top:parent.top
            bottom:parent.bottom
            left:parent.left
            right:parent.right
            margins: appStyle.margins
        }
        spacing: 5
        StyledText {
            id: name
            text: "Orbiter "+ manager.m_dwPK_Device + " is connected "
            fontSize: mediumText
            color: "white"
        }
    }


    Keys.onTabPressed:swapFocus()
    onActiveFocusChanged: {
        if(activeFocus)
            console.log("Header gained focus")
        else
            console.log("Header lost focus")
    }
    states: [
        State {
            name: "hidden"

            AnchorChanges{
                target:hdr
                anchors.top: undefined
                anchors.bottom: qmlroot.top
            }
        },
        State {
            name: "showing"
            AnchorChanges{
                target:hdr
                anchors.bottom: undefined
                anchors.top: qmlroot.top
            }
        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "*"

            AnchorAnimation{
                duration: appStyle.globalAnimationSpeed
                easing.type: appStyle.globalAnimationEasing

            }
        }
    ]

}
