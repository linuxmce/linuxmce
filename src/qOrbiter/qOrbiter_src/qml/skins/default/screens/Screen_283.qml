import QtQuick 2.3
import "../components"
import "../"
StyledScreen{
    id:message
    screen:qsTr("Cannot Reload Router")
//    screen_params: [
//        9 /* The main message followed by up to 4 messages, all | delimited */,
//        137 /* Up to 4 messages for the 4 options, | delimited */,
//        163 /* A short keyword explaining what the message in for in case there's some special code to intercept it */,
//        181 /* Prompt To Reset Router */,
//        182 /* Timeout */,
//        183 /* Cannot Go Back */
//    ]

    Row{
        width: parent.width/2
        height: 200
        anchors.centerIn: parent

        StyledButton{
            buttonText: qsTr("Force Reload")
            onActivated:  manager.forceReload()
        }

        StyledButton{
            buttonText: qsTr("Home")
        }
    }


}
