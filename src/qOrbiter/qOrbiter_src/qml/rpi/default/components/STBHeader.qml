import QtQuick 2.2
import QtGraphicalEffects 1.0
import "../../../skins-common/lib/components"

Item{
    id:hdr
    anchors.bottom: parent.top
    width: qmlroot.width
    height:scaleY(8)
    state:"hidden"
    property bool isActive: activeFocus
    Rectangle{
        anchors.fill: parent
        color:appStyle.mainColor
        border.width: hdr.activeFocus ? 2 : 0
        focus:false
        border.color:"red"
        opacity: hdr.activeFocus ? 1 : .65
    }

    Row{
        height: parent.height
        width: parent.width
        spacing: 5

        StyledText {
            id: name
            text: "Orbiter "+ manager.iPK_Device + " is connected to "+ manager.m_ipAddress
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
