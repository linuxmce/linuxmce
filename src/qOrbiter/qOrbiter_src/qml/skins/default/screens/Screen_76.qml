import QtQuick 2.2
import "../components"
import "../."

StyledScreen {

    screen: "Power"

    Panel{
        headerTitle: qsTr("Power and Reboot Options")
        Flow{
            width: appStyle.appButtonWidth *2+10
            height: appStyle.appButtonHeight *2+10
            spacing: 4
            anchors.centerIn: parent

            StyledButton{
                buttonText:dcenowplaying.b_mediaPlaying ? qsTr("Stop Media:%1").arg("\n"+dcenowplaying.mediatitle): qsTr("Power off Display")
                onActivated: {
                    if(dcenowplaying.b_mediaPlaying)
                        manager.stopMedia()
                    else
                        manager.toggleDisplay(false)
                }
            }
            StyledButton{
                buttonText: qsTr("Power off MD")
            }
            StyledButton{
                buttonText: qsTr("Reboot Core")
            }
            StyledButton{
                buttonText: qsTr("Reboot MD")
            }
        }

    }



}
