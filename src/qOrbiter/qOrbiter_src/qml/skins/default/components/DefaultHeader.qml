import QtQuick 2.3
import QtGraphicalEffects 1.0
import "../"
/*! This File is designed to be the main layout that can be switched in and out for various forms */
Item{
    id:header
    height: Style.appNavigation_panelHeight

    state:uiOn ? "open" : "closed"
    focus:true
    activeFocusOnTab: false
    onActiveFocusChanged: {
        console.log("Header has focus ? "+ activeFocus)

    }

    Keys.onTabPressed: {
        footer.forceActiveFocus()
    }
    anchors{
        left:layout.left
        right:layout.right
    }

    Rectangle{
        id:bg_fill
        anchors.fill: parent
        color:Style.appcolor_background_light
        opacity: Style.appHeader_opacity        
    }

    Glow{
        anchors.fill:bg_fill
        radius:8
        samples:16
        color:Style.apptext_color_active
        source:bg_fill
        visible:header.activeFocus
    }

    
    states: [
        State {
            name: "open"
            AnchorChanges{
                target:header
                anchors.bottom: undefined
                anchors.top: layout.top
            }
        },
        State {
            name: "closed"
            AnchorChanges{
                target:header
                anchors.top: undefined
                anchors.bottom: layout.top
            }
        }
    ]
    
    transitions: [
        Transition {
            from: "*"
            to: "*"
            AnchorAnimation{
                duration: Style.transition_accentTime
            }
        }
    ]
}
