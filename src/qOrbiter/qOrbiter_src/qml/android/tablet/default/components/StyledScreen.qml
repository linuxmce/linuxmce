import QtQuick 1.1

Item {
    id:screen_root

    focus:true
    opacity: 0
    Component.onCompleted: {
        setNavigation(navigation)
        forceActiveFocus()
        state="opening"
        screenOpening()
    }
    Behavior on opacity {
        PropertyAnimation{
            duration: 350
        }
    }

    onOpacityChanged: {
        if(opacity==0 && state=="closed"){
            readyToClose()
        } else if(opacity==1 && state=="opened"){
            screenOpening()
        }
    }

    onStateChanged: {
        if(state=="closing"){
            screenClosing()
        }
    }

    function close(){
        state="closing"
    }

    function setNavigation(navFile){
        nav_row.navSource=navFile
    }

    property string screen:"screenum ipsum"
    property string navigation:"ScenarioComponent.qml"
    signal readyToClose()
    signal screenClosing()
    signal screenOpening()

    onReadyToClose: {
        pageLoader.loadNext();
    }

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
                opacity:1
            }

        },
        State {
            name: "opened"
            PropertyChanges {
                target: screen_root
                when:opacity==1
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
            when:opacity==0
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
