import QtQuick 2.2
import QtGraphicalEffects 1.0
import "."

/*!
This File is designed to be the main layout that can be switched in and out for various forms
the exception is the STB footer, which will operate differently
*/
Item{
    id:footer
    focus:true
    property bool activated:false
    anchors{
        left:parent.left
        right:parent.right
    }
    activeFocusOnTab: false
    onActiveFocusChanged: {
        console.log("Footer has focus ? "+ activeFocus)
    }

    Keys.onTabPressed: {
        layout.forceActiveFocus()
    }

    height: Style.appNavigation_panelHeight
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
        visible:footer.activated
    }

    states: [
        State {
            when:manager.currentScreen==="Screen_1.qml" && !screensaverActive
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
            when:manager.currentScreen!=="Screen_1.qml" ||  qmlRoot.screensaverActive
            AnchorChanges{
                target: footer
                anchors{
                    bottom:undefined
                    top:layout.bottom
                }
            }
        }, State {
            name: "retracted"
            PropertyChanges {
                target: object

            }
        }
    ]
    transitions: [
        Transition {
            from: "*"
            to: "*"
            AnchorAnimation{
                duration: Style.transition_accentTime
                easing.type: Easing.OutExpo
            }
        }
    ]
}
