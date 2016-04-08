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
        property string requestUrl: ("http://"+manager.currentRouter+"/lmce-admin/qOrbiterGenerator.php?c=")

        function getParams(cmd_no, idx){

            var paramsRequest = new XMLHttpRequest();
            paramsRequest.onreadystatechange = function(){
                if(paramsRequest.readyState===XMLHttpRequest.DONE){
                        processParamCallback(paramsRequest.responseText)
                }
            }
           var finalUrl = requestUrl+(cmd_no+"&idx="+idx)
             console.log(finalUrl)
            paramsRequest.open("POST",finalUrl)
            paramsRequest.send();
        }



        function processParamCallback(data){
            var p1 = JSON.parse(data)
            var device_id = p1.device.device
            var params = p1.params

            var lp = params

            floorplan_devices.setDeviceParams(lp, device_id)
          //  console.log(JSON.stringify(params))
//            for (var p in params){
//                console.log(params[p].CommandDescription+":"+params[p].CommandHint)
//            }


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

    Keys.onEscapePressed: manager.setCurrentScreen("Screen_1.qml")

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
