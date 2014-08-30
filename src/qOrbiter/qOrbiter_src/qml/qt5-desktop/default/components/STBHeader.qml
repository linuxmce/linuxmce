import QtQuick 2.1
import "../../../skins-common/lib/components"

Item{
    id:hdr
    anchors{
        left:qmlroot.left
        right:qmlroot.right
    }
    height:scaleY(8)
    state:"hidden"
    property bool isActive: false

    GradientFiller {
        opacity: .75

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
            text: "Orbiter "+ manager.m_dwPK_Device + " is connected to" + manager.m_ipAddress
            fontSize: mediumText
            color: "white"
        }
        StyledText {
            id: mp
            text: "Mediaplayer "+ manager.mediaPlayerID + (dceplayer.online ? " is connected " : " is offline.")
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
            when:!uiOn
            AnchorChanges{
                target:hdr
                anchors.top: undefined
                anchors.bottom: qmlroot.top
            }
        },
        State {
            name: "showing"
            when:uiOn
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
