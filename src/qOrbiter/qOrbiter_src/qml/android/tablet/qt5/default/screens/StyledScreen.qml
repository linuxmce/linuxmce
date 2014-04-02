import QtQuick 2.0

Item {
    id:screen_root
    state:"opening"
    Component.onCompleted: {
        state="opened"
    }

    function close(){
        state="closing"
    }

    property string screen:"screenum ipsum"
    signal readyToClose()
    signal screenClosing()
    signal opening()

    anchors{
        top:pageLoader.top
        left:pageLoader.left
        right:pageLoader.right
        bottom:pageLoader.bottom
    }

    states: [
        State {
            name: "opening"
            PropertyChanges {
                target: screen_root
                opacity:0
            }
            ScriptAction{
                script: { opening() }
            }
        },
        State {
            name: "opened"
            PropertyChanges {
                target: screen_root
                opacity:1
            }
        },
        State {
            name: "closing"
            PropertyChanges {
                target: screen_root
                opacity:0
            }
            ScriptAction{
                script: {screenClosing() }
            }
        },
        State {
            name: "closed"
            ScriptAction{
                script: { readyToClose() }
            }
        }
    ]
}
