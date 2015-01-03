import QtQuick 2.2

Item{
    id:nav_row
    height: scaleY(8)
    state:"extended"
    property double loadingProgress: nav.progress
    onLoadingProgressChanged: console.log(loadingProgress)
    property alias navigation:nav
    property string defaultSource:"ScenarioComponent.qml"
    property string navSource:"ScenarioComponent.qml"
    signal goingBack()
    anchors{
        top:qml_root.top
        left:qml_root.left
        right:qml_root.right

    }

    Rectangle{
        anchors.fill: nav_row
        color:manager.connectedState ? style.darkColor : style.alertcolor
        opacity:style.shadeOpacity
    }

    Loader{
        id:nav
        source:navSource        
        anchors{
            top:parent.top
            left: parent.left
            right:commonButtons.left
            bottom:parent.bottom
            rightMargin: scaleX(1)
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


    }


    Flickable{
        id:commonButtons
        height: parent.height
        width: manager.appWidth *.30
        anchors.right: parent.right


        Row{
            width: 5*50
            height: childrenRect.height
            spacing:scaleX(1)
            anchors.verticalCenter: parent.verticalCenter
            StyledButton{
                buttonText:"Advanced"
                width: scaleX(14)
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
                }
                visible: manager.currentScreen==="Screen_47.qml"
            }
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
        },
        State {
            name: "lowered"
            AnchorChanges{
                target: nav_row
                anchors{
                    top:undefined
                    bottom:parent.top
                }
            }
        },
        State {
            name:"raised"
            AnchorChanges{
                target: nav_row
                anchors{
                    bottom:undefined
                    top:parent.top
                }
            }
        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "*"
            AnchorAnimation{
                duration: 350
                easing.type: Easing.InCurve
            }
        }
    ]
}
