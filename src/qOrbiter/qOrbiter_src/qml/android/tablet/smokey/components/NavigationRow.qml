import QtQuick 1.1
import "../../../../skins-common/lib/handlers"
Item{
    id:nav_row
    height: scaleY(8)
    state:"extended"

    property alias navigation:nav
    property string defaultSource:"ScenarioComponent.qml"
    property string navSource:"ScenarioComponent.qml"
    anchors{
        left:qml_root.left
        right:qml_root.right
    }

    Rectangle{
        anchors.fill: parent
        color:style.darkColor
        opacity:style.shadeOpacity
    }

    Row{
        id:layout_row
        height: scaleY(8)
        anchors{
            left:parent.left
            right:parent.right
        }


        spacing:scaleX(2)

        Loader{
            id:nav
            source:navSource
            anchors{
                top:parent.top
                bottom:parent.bottom
                left: parent.left
                right:parent.right
            }
            anchors.rightMargin: parent.width*.25
        }

        StyledButton{
            buttonText:"Advanced"
            opacity: manager.currentScreen === "Screen_1.qml" ? 1 : 0
            onActivated: manager.gotoQScreen("Screen_44.qml")
        }

        StyledButton {
            id: exit_label
            buttonText: qsTr("Exit")
            hitArea.onReleased: manager.exitApp()
            opacity:manager.currentScreen ==="Screen_1.qml" ? 1 : 0
        }
        StyledButton {
            id: home_label
            buttonText: qsTr("Home")
            hitArea.onReleased: manager.gotoQScreen("Screen_1.qml")
            opacity: manager.currentScreen !=="Screen_1.qml" ? 1 : 0
        }
        StyledButton{
            id:media_goback
            buttonText: "Back"
            hitArea.onReleased:{
                manager.goBackGrid();

                //            else{

                //                pageLoader.item.state="selection"
                //                manager.goBackGrid()
                //                mediatypefilter.reset()
                //            }
            }
            visible: manager.currentScreen==="Screen_47.qml"
        }
    }
    states: [
        State {
            name: "extended"
            when:uiOn
            PropertyChanges {
                target: nav_row
                height:scaleY(8)
            }
        },
        State {
            when:!uiOn
            name: "retracted"
            PropertyChanges {
                target: nav_row
                height:0
            }
        }
    ]
}
