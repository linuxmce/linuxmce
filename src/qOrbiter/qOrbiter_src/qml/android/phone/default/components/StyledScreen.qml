import QtQuick 1.1

Item {
    id:screen_root
    state:"opening"
    Component.onCompleted: {
        state="opened"
        console.log(manager.currentScreen+" is open.")
      var o = String(manager.currentScreen)
        setOptions(o.replace("Screen","HomeOptions"))
    }

    function close(){
        state="closing"
    }

    property string screen:"screenum ipsum"
    signal readyToClose()
    signal screenClosing()
    signal screenOpening()

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

        },
        State {
            name: "closed"

        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "*"
            PropertyAnimation{
                duration: 500
            }
        }
    ]
}
