import QtQuick 2.2
Item{
    id:paramManager


           property string requestUrl: ("http://"+manager.m_ipAddress+"/lmce-admin/qOrbiterGenerator.php?c=")

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
//               for (var p in params){
//                   console.log(params[p].CommandDescription+":"+params[p].CommandHint)
//               }
           }
}
