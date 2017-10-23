import QtQuick 2.1

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
            margins: appstyle.margins
        }
        spacing: 5
        StyledText {
            id: name
            text: "Orbiter "+ manager.iPK_Device + " is connected to" + manager.currentRouter
            fontSize: mediumText
            color: "white"
        }
        StyledText {
            id: mp
            text: "Mediaplayer "+ manager.mediaPlayerID + (dceplayer.online ? " is connected " : " is offline.")
            fontSize: mediumText
            color: "white"
        }

        StyledText {
            id: dp
            text: "current screen "+ screenInfo.primaryScreen.logicalDpi
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
                duration: appstyle.globalAnimationSpeed
                easing.type: appstyle.globalAnimationEasing

            }
        }
    ]

}
