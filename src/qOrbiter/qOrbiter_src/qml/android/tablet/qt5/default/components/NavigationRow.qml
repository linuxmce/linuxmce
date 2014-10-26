import QtQuick 2.0
import "../../../../../skins-common/lib/handlers"

Row{
    id:nav_row
    height: scaleY(8)
    anchors{
        left:parent.left
        right:parent.right
        top:parent.top
    }
    Connections{
        target:qml_root
        onShowUi:{
            if(uiState){
                state="active"
            }else{
                state="hidden"
            }
        }
    }
    property alias navigation:nav
    property string defaultSource:"ScenarioComponent.qml"
    property string navSource:"ScenarioComponent.qml"
    spacing:scaleX(2)
    Loader{
        id:nav
        source:navSource
        height: parent.height
        width: item.width
    }
    StyledButton{
        buttonText.text:"Advanced"
        opacity: manager.currentScreen === "Screen_1.qml" ? 1 : 0
    }

    StyledButton {
        id: exit_label
        buttonText.text: qsTr("Exit")
        hitArea.onReleased: manager.exitApp()
        opacity:manager.currentScreen ==="Screen_1.qml" ? 1 : 0
    }
    StyledButton {
        id: home_label
        buttonText.text: qsTr("Home")
        hitArea.onReleased: manager.setCurrentScreen("Screen_1.qml")
        opacity: manager.currentScreen !=="Screen_1.qml" ? 1 : 0
    }
    StyledButton{
        id:media_goback
        buttonText.text: "Back"
        hitArea.onReleased:{ manager.goBackGrid()
        }
        visible: manager.currentScreen==="Screen_47.qml"
    }


    states: [
        State {
            name: "active"
           AnchorChanges{
               target: nav_row
               anchors{
                   bottom:undefined
                   top:parent.top
               }
           }
        },
        State {
            name: "hidden"
            AnchorChanges{
                target: nav_row
                anchors{
                    top:undefined
                    bottom:parent.top
                }
            }
        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "*"
            AnchorAnimation{
                duration:style.quickAnimation
                easing.type:style.animationEasing
            }
        }
    ]
}
