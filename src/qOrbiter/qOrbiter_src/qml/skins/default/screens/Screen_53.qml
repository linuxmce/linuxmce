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
        content:Item{
            id:messageContainer
            function formatText(){
                var t1 = screenparams.getParam(163)


            }

            StyledText{
                id:messageLabel
                text: screenparams.getParam(163)
                font.pointSize: largeFontSize
                anchors{
                    top:parent.top
                    horizontalCenter: parent.horizontalCenter
                }
                font.bold: true
            }

            Row{
                id:reloadOption
                width: parent.width/2
                anchors{
                    bottom:parent.bottom
                    horizontalCenter: parent.horizontalCenter
                }

                visible: screenparams.getParam(181) ==1
                StyledButton{
                    visible:parent.visible
                    buttonText:qsTr("Reload Router")
                }
                StyledButton{
                    visible:parent.visible
                    buttonText:qsTr("Reload Later")
                }
            }


        }
    }

}
