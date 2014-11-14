import QtQuick 2.0

Item {
    id:screen_root
    state:"opening"
    opacity:0

    Behavior on opacity {
        PropertyAnimation{ duration: 250}
    }

    onOpacityChanged: {
        if(state=="closing" && opacity==0){
            close()
        }
    }

    Component.onCompleted: {
        opacity=1
        console.log(manager.currentScreen+" is opening")
        setNavigation(navigationComponent)
        if(manager.currentScreen != "Screen_1.qml"){
             lowerInfoPanel();
        } else {
            showInfoPanel();
        }


        //info_panel.state="retracted";
    }

    function close(){
        console.log("Loader Request close")
        state="closed"
        pageLoader.loadNext();
    }

    property string screen:manager.currentScreen
    signal readyToClose()
    signal screenClosing()
    signal screenOpening()
    property string navigationComponent:""

    onReadyToClose: {
        console.log("Closed, loading next!");
        pageLoader.loadNext()
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
                opacity:0
            }
        },
        State {
            name: "opened"
            when:screen_root.opacity==1
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
            when:screen_root.opacity==0
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
