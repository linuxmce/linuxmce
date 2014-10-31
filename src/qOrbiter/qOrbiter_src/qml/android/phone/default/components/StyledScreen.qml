import QtQuick 1.1

Item {
    id:screen_root
    state:"opening"
    property string navigation:""
    property string screen:"screen um ipsum lorem"
    signal readyToClose()
    signal screenClosing()
    signal screenOpening()
    signal screenOpened()

    anchors{
        top:pageLoader.top
        left:pageLoader.left
        right:pageLoader.right
        bottom:pageLoader.bottom
    }

    Component.onCompleted: {
        screen=manager.currentScreen

        if(navigation!==""){
            setNavigation(navigation)
        }

        console.log(manager.currentScreen+" is open.")
        var o = String(manager.currentScreen)
        setOptions(o.replace("Screen","HomeOptions"))
        state="opened"

    }

    function close(){
        state="closing"
    }

    states: [
        State {
            name: "opening"
            StateChangeScript{
                script: {console.log(screen+" is opening");screenOpening(); }
            }

        },
        State {
            name: "opened"
            StateChangeScript{
                script: {console.log(screen+" is opened");screenOpened()}
            }

        },
        State {
            name: "closing"
            StateChangeScript{
                script: {console.log(screen+" is closing");screenClosing()}
            }

        },
        State {
            name: "closed"
            StateChangeScript{
                script: {console.log(screen+"is closed");readyToClose()}
            }

        }
    ]


}
