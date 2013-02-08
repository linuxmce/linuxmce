
import QtQuick 1.0

Rectangle {

    signal switchState( bool l_status)
    id: switchbutton
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
                switchState(false)
                }

            else
                {switchbutton.state ="ON"
                 switchState(true)
                }

                 }
        onExited: {
            //switchManager.writeLog (switchbutton.state)
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
