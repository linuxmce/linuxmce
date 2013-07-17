import QtQuick 1.1
import "../components"

Item {
    width: scaleY(100)
    height: scaleX(100)
    state:"list"
    focus:true
    onStateChanged: {
        if(state==="list"){
            listdisplay.forceActiveFocus()
        }
        else{
            floorplan.forceActiveFocus()
        }
    }

    property bool image:false
    onActiveFocusChanged: {
        console.log("Floorplan has focus")
        if(state==="list"){
            listdisplay.forceActiveFocus()
        }
        else{
            floorplan.forceActiveFocus()
        }
    }

    Keys.onEscapePressed: manager.gotoQScreen("Screen_1.qml")

//    Rectangle{
//        id:fil
//        height: manager.appHeight
//        width: manager.appWidth
//        color:"black"
//        opacity: .65
//        anchors.centerIn: parent
//    }

    FloorplanDisplay{
        id:floorplan
    }

    ListDisplay{
       id:listdisplay
    }

    states: [
        State {
            name: "list"
            PropertyChanges {
                target: floorplan
                visible:false
            }
            PropertyChanges {
                target: listdisplay
                visible:true

            }
        },
        State {
            name: "floorPlan"
            PropertyChanges {
                target: floorplan
                visible:true
            }
            PropertyChanges {
                target: listdisplay
                visible:false
            }
        }
    ]


}
