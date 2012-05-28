import QtQuick 1.0
import "../components"


Rectangle {
    id:stage
    function toggleUI()
    {
        mediaHeader.visible = !mediaHeader.visible
        lmceScenarios.visible = !lmceScenarios.visible
    }
    function keyCheck(k){
        if(k === Qt.Key_T)
            toggleUI()
        console.log("Key toggle")

    }
    signal swapStyle()
    height: appH
    width: appW
    color: "black"
    //focus:true



    MediaHeader{id:mediaHeader}
    //BottomPanel{id: advanced; anchors.top: stage.top}
    ScenarioRow{id:lmceScenarios; anchors.bottom: stage.bottom}


}



