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
    focus:true
    Keys.onPressed:{
        console.log(event.key)
                keyCheck(event.key)
    }

    MediaHeader{id:mediaHeader}
    //BottomPanel{id: advanced; anchors.top: stage.top}
    ScenarioRow{id:lmceScenarios; anchors.bottom: stage.bottom}

    MouseArea{
        anchors.fill: stage
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        hoverEnabled: true
        onClicked:  {
            //  console.log("stage click")
            if(mouse.button === Qt.RightButton)
            {
                stage.toggleUI()
                //  console.log("Mouse Toggle")
            }
        }
    }
}



