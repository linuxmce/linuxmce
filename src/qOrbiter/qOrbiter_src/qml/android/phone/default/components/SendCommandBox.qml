import QtQuick 1.0
import "../components"
Item{
    id:sendCommandBox
    height: manager.appHeight
    width: manager.appWidth
    focus:true
    clip:true
    property Item trackedDevice
    property int trackedInt:0

    Connections{
        target:floorplan_devices
        onDeviceCommandsChanged:{
            console.log(JSON.stringify(floorplan_devices.getDeviceCommands(trackedInt)))
           availbleCommands.clear()
            var cl = floorplan_devices.getDeviceCommands(trackedInt).commands
            for(var c in cl){
                availbleCommands.append(cl[c])
            }


        }
    }

    onActiveFocusChanged: {
        console.log("Cmd box  focus"+activeFocus)
        if(activeFocus){
            trackedInt = selections.get(0).device
            manager.getFloorplanDeviceCommands(trackedInt)
        }
    }

    ListModel{
        id:availbleCommands
    }


    Keys.onReleased: {
        event.accepted = true
        switch(event.key){

        case Qt.Key_Menu:
            phoneFloorplanLayout.state="floorplanView"
            console.log("hiding command view")
            break;
        default:
            console.log("Cmd box Recieved key ==>"+event.key+" but not accepting")
            break;
        }

    }

    Rectangle{
        id:phil
        anchors.fill: parent
        color:"black"
        opacity: .85
    }

    Rectangle{
        id:controlsContainer
        height: parent.height*.75
        width: parent.width*.95
        color: androidSystem.greenStandard
        radius: 5
        border.color: "white"
        anchors.centerIn: parent
        visible:parent.visible
    }

    StyledText {
        id: cmdsPrmoptText
        text: qsTr("Availible Commands")
        fontSize: scaleY(3)
        color: "Black"
        anchors.top: controlsContainer.top
        anchors.horizontalCenter: controlsContainer.horizontalCenter
        anchors.topMargin: 10
    }
    ListView{
        id:cmdView
        anchors.top: cmdsPrmoptText.bottom
        width: controlsContainer.width -5
        height: controlsContainer.height - cmdsPrmoptText.height - 10
        anchors.horizontalCenter: controlsContainer.horizontalCenter
        model:availbleCommands
        spacing:scaleY(2)
        delegate: Rectangle{
            height: 75
            width: parent.width
            color: "black"
            border.color: "white"
            border.width: 1
            radius:5
            StyledText{
                text:command_name
                fontSize: 22
                color:"white"
                anchors.centerIn: parent
            }
        }
    }

}
