import QtQuick 2.2
import "../components"
/*!
  Device Reload Screen
  */
StyledScreen{
    id:message
    screen_params: [
    9 /* The main message followed by up to 4 messages, all | delimited */,
    137 /* Up to 4 messages for the 4 options, | delimited */,
   163 /* A short keyword explaining what the message in for in case there's some special code to intercept it */,
   181 /* Prompt To Reset Router */,
   182 /* Timeout */,
   183 /* Cannot Go Back */
    ]

    Panel{
        headerTitle: qsTr("System Message")
        StyledText{
            text: screenparams.getParam(9)
            font.pointSize: largeFontSize
            anchors.centerIn: parent
            font.bold: true
        }
    }

}
