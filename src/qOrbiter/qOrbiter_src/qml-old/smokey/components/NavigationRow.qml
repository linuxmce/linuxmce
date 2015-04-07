import QtQuick 1.1
import "../../../../skins-common/lib/handlers"
Item{
    id:nav_row
    height: scaleY(8)
    state:"extended"
    property double loadingProgress: nav.progress
    onLoadingProgressChanged: console.log(loadingProgress)
    property alias navigation:nav
    property string defaultSource:"ScenarioComponent.qml"
    property string navSource:"ScenarioComponent.qml"
    anchors{
        top:qml_root.top
        left:qml_root.left
        right:qml_root.right
    }

    Rectangle{
        anchors.fill: parent
        color:manager.connectedState ? style.darkColor : style.alertcolor
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
            onSourceChanged: {
                if(nav.status===Component.Ready){
                    console.log("Header Navigation loaded")
                }else if(nav.status===Component.Loading){
                    console.log("Header Navigation Loading")
                } else if(nav.status===Component.Error){
                    console.log("Header Navigation failed to load, falling back.")
                }
            }

            anchors.rightMargin: parent.width*.25
        }

        StyledButton{
            buttonText:"Advanced"
            opacity: manager.currentScreen === "Screen_1.qml" ? 1 : 0
            onActivated: manager.setCurrentScreen("Screen_44.qml")
        }
        StyledButton {
            id: showFloorplanCommand
            buttonText: qsTr("Commands")
            hitArea.onReleased: pageLoader.item.state==="commandView" ?pageLoader.item.state="floorplanView" :pageLoader.item.state="commandView"
            visible:navSource==="FloorplanNav.qml" &&  pageLoader.item.selectedDevices.count !== 0? true : false
            opacity: visible ? 1 : 0
        }

        StyledButton {
            id: exit_label
            buttonText: qsTr("Exit")
            hitArea.onReleased: manager.closeOrbiter()
            opacity:manager.currentScreen ==="Screen_1.qml" ? 1 : 0
        }
        StyledButton {
            id: home_label
            buttonText: qsTr("Home")
            hitArea.onReleased: manager.setCurrentScreen("Screen_1.qml")
            opacity: manager.currentScreen !=="Screen_1.qml" ? 1 : 0
        }

        StyledButton{
            id:media_goback
            buttonText: "Back"
            hitArea.onReleased:{
                manager.goBackGrid();

                //            else{

                //                pageLoader.item.sztate="selection"
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

            AnchorChanges{
                target: nav_row
                anchors{
                    bottom:undefined
                    top:parent.top
                }
            }
        },
        State {
            when:!uiOn
            name: "retracted"

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
                duration: 500
                easing.type: Easing.InCurve
            }
        }
    ]
}
