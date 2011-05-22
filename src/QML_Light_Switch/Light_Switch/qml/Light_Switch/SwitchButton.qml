
import QtQuick 1.0

Rectangle {
    signal writeLog(string msg)
    id: switchbutton

   /* Connections {
    target:switchManager
    onDCEon:  switchbutton.state = "ON"
    onDCEoff: switchbutton.state = "Off"
    } */

    width: 100; height: 100;
    color: "yellow"
    state: "OFF"
    Image {
        height: 100
        width: 100
    id: lt_on
         }

    MouseArea {
        height: 100; width: 100
        anchors.fill: parent
        onClicked: {

            if (switchbutton.state =="ON")
                {switchbutton.state = "OFF"
                switchManager.writeLog("Hello From QML- Switch turned off")}

            else
                {switchbutton.state ="ON"
                switchManager.writeLog("Hello From QML- Switch turned On")}

                 }
        onExited: {console.log(switchbutton.state)
                switchbutton.writeLog (switchbutton.state)
              }
    }

    Text {
        id: lightLabel
        color: "white"
        text: "button"
    }
    states:
        [
        State {
            name:"ON"
            PropertyChanges {target:switchbutton; color: "green"}
            PropertyChanges {target: lightLabel; text: "On!"}
            PropertyChanges {target: lt_on; source:"qrc:/sconces/on.png"}

            },

        State {
        name:"OFF"
        PropertyChanges {target: switchbutton; color: "red";}
        PropertyChanges {target: lightLabel; text: "Off!"}
        PropertyChanges {target: lt_on; source: "qrc:/sconces/off.png"}
             }
          ]
}
