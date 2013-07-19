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

    Item {
        id: requestParamManager
        property string requestUrl: ("http://"+manager.m_ipAddress+"/lmce-admin/qOrbiterGenerator.php?c=")

        function getParams(cmd_no){

            var paramsRequest = new XMLHttpRequest();
            paramsRequest.onreadystatechange = function(){
                if(paramsRequest.readyState===XMLHttpRequest.DONE){
                        processParamCallback(paramsRequest.responseText)
                }
            }
            paramsRequest.open("POST",(requestUrl+cmd_no))
            paramsRequest.send();
        }



        function processParamCallback(data){
            console.log(requestUrl)
            //console.log(JSON.stringify(data))

            var params = JSON.parse(data)
            for (var p in params){
                console.log(params[p].CommandDescription+":"+params[p].CommandHint)
            }

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

    function createProxy(){
        console.log("Getting proxy model.")
        console.log(floorplan_devices.count)
        for(var i = 0; i < floorplan_devices.count; i++){
            if(floorplan_devices.get(i).floorplantype===2){
                console.log("sucess")
            }
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
       floorplanType: 2
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
