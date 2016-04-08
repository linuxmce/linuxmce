import QtQuick 2.2
import "../."
Item {
    id:screen_root
    focus:true
    opacity: 0
    anchors.fill: parent
    Keys.onTabPressed: header.forceActiveFocus()
    property variant screen_params:[]
    Timer{
        id:failSafe
        interval: 8000
        onTriggered: state="open"

    }

    Component.onCompleted: {
        console.log("screen opening")
        forceActiveFocus()
        state="opening"
        screenOpening()
    }


    onActiveFocusChanged:{
        if(activeFocus){
            console.log("screen_root has focus")
            raiseNavigation(false)
        }

    }
    Behavior on opacity {
        PropertyAnimation{
            duration: 300
        }
    }

    onScreenOpening: {
        state="opened"
        //raiseNavigation(false)
        //        raiseNavigation(manager.currentScreen==="Screen_1.qml" ? true :keepHeader )
        //        if(keepHeader){
        //            setNavigation(navigation)
        //        }

        for(var param in screen_params){

            if(param)
            console.log("Screen parameter property %1 -- %2").arg(param).arg(screenparams.getParam(param))
        }
    }

    onOpacityChanged: {
        if(opacity===0 &&state==="closing"){
            state="closed"
            readyToClose()
        }
    }

    onStateChanged: {

        if(state==="closed"){
            console.log("screen closed.")

        } else if(state==="closing"){
            raiseNavigation(true)
        }
    }

    function setNavigation(navFile){
        // nav_row.navSource=navFile
    }

    property string screen:"screenum ipsum"
    readonly property bool styledScreen:true
    property string navigation:"ScenarioComponent.qml"
    property bool keepHeader:true
    property bool keepFooter:false;
    property bool noForce:false
    signal readyToClose()
    signal screenClosing()
    signal screenOpening()
    signal screenOpen()

    onReadyToClose: {
        pageLoader.loadNext();
    }


    states: [
        State {
            name: "opening"
            PropertyChanges {
                target: screen_root
                opacity:.5
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
                duration: Style.transition_animationTime
            }
        }
    ]
}
