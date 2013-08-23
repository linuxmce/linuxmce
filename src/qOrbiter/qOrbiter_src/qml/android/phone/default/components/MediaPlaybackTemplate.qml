import QtQuick 1.1

Item {
    width: manager.appWidth
    height: manager.appHeight
    property Item controlSourceItem:AudioRemote{}


    Loader{
        id:controlsTarget
        anchors.bottom: parent.bottom
        sourceComponent:sourceControlItem
    }

    state:"controls"

    Item{
        id:imgContainer
    }

    states: [
        State {
            name: "controls"
            PropertyChanges {
                target: imgContainer
                visible:true
            }
        },
        State{
            name:"playlist"
            PropertyChanges{
                target:imgContainer
                visible:false
            }
        },
        State{
            name:"advanced"
            PropertyChanges {
                target: imgContainer
                visible:false
            }
        }

    ]
}
