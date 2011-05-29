import QtQuick 1.0

Rectangle {
    width: 360
    height: 360
    color: "black"
    opacity: 0.75
    id: playfield
    signal close_app()
    signal l_state( string light_state)

    Text {
        id: switchpanel

        text: "DCE / QML Light Switch "
        color: "white"
        anchors.centerIn: parent
         }
    MouseArea {
        height: 50; width: 360;
        anchors.bottom: switchpanel.bottom
        onClicked: {  playfield.close_app()   }

              }

    function dceCommand(msg,val)
    { console.log("Recieved command in function, parsing: ")
        if (msg =="ON")
            {
            switchbutton.state="ON"
            console.log("Recieved DCE ON in qml")
        }
        else if (msg=="OFF"){
            switchbutton.state="OFF"
            console.log("Recieved DCE OFF in qml")
        }
            else if (msg =="SETLEVEL")
            {
                (console.log("Setting light to: " + val))
        }
        return true;
    }


    //defining the button itself
    Rectangle {

        signal switchState( bool l_status)
        id: switchbutton
        width: 100; height: 100;
        color: "yellow"
        state: "ON"

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
                    l_state("Light State: " + switchbutton.state)

                    }

                else
                    {switchbutton.state =="OFF"
                    switchbutton.state = "ON"
                     l_state("Light State: " + switchbutton.state)
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
           }

