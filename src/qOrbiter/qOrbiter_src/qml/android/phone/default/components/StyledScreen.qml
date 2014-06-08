import QtQuick 1.1

Item {
    id:screen_root

    property string navigation:""
        anchors{
            top:pageLoader.top
            left:pageLoader.left
            right:pageLoader.right
            bottom:pageLoader.bottom
        }

    Component.onCompleted: {
        state="opening"
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

    property string screen:"screenum ipsum"
    signal readyToClose()
    signal screenClosing()
    signal screenOpening()



    states: [
        State {
            name: "opening"
            StateChangeScript{
                script: {console.log(screen+"is opening")}
            }

        },
        State {
            name: "opened"
            StateChangeScript{
                script: {console.log(screen+"is opened")}
            }

        },
        State {
            name: "closing"
            StateChangeScript{
                script: {console.log(screen+"is closing")}
            }

        },
        State {
            name: "closed"
            StateChangeScript{
                script: {console.log(screen+"is closed")}
            }

        }
    ]


}
