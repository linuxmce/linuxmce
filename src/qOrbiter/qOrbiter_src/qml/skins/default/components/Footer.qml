import QtQuick 2.3
import "."

/*! This File is designed to be the main layout that can be switched in and out for various forms */
Item{
    id:footer
    anchors{
        left:parent.left
        right:parent.right
    }
    height: Style.appNavigation_panelHeight
    Rectangle{
        anchors.fill: parent
        color:Style.appcolor_background_light
        opacity: Style.appHeader_opacity
    }
    
    states: [
        State {
            when:uiOn
            name: "open"
            AnchorChanges{
                target: footer
                anchors{
                    top:undefined
                    bottom:layout.bottom
                }
            }
        },
        State {
            name: "closed"
            when:!uiOn
            AnchorChanges{
                target: footer
                anchors{
                    bottom:undefined
                    top:layout.bottom
                }
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
