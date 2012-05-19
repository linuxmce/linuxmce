import QtQuick 1.0
import "../components"

FocusScope{
    Item
    {
        anchors.centerIn: parent
        function toggleUI()
        {
            advanced.visible = !advanced.visible
            lmceScenarios.visible = !lmceScenarios.visible
        }
        function keyCheck(k){
            if(k === Qt.Key_T)
                toggleUI()
            console.log("Key toggle")

        }


        id:testScope
        Rectangle {
            id:stage
            signal swapStyle()
            height: appH
            width: appW
            color: "black"
            /*
        Image {
            id: headerbg
            source: "../img/ui3/header.png"
            anchors.top: stage.top
            height: scaleY(7)
            width: scaleX(99)
        }
 */

            Rectangle{
                id: temp_nowplaying
                width: scaleX(50)
                height: scaleY(35)
                color: "grey"
                anchors.top: advanced.bottom
                anchors.topMargin: scaleY(15)
                radius: 10
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: label
                    text: dcenowplaying.mediatitle
                }
            }
            BottomPanel{id: advanced; anchors.top: stage.top}
            ScenarioRow{id:lmceScenarios; anchors.bottom: stage.bottom}
        }
        Keys.onPressed:keyCheck(event.key)
    }


    MouseArea{
        anchors.fill: testScope
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked:  {
            if(mouse.button === Qt.RightButton)
            {
                toggleUI()
                console.log("Mouse Toggle")
            }
        }
    }
}
