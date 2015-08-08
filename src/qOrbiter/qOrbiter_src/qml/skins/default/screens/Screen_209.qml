import QtQuick 2.2
import "../components"
StyledScreen {
    screen: qsTr("Need to Regenerate")


    Flow{
        anchors.centerIn: parent
        width: parent.width *.65
        height: parent.height*.65

        StyledButton{
            buttonText: qsTr("Regenerate This orbiter")
            /*CMD_REGEN_ORBITER */
        }
        StyledButton{
            buttonText: qsTr("Regenerate All Orbiters")
            /*  CMD_Regen_Orbiter_Cat */

        } StyledButton{
            buttonText: qsTr("Dont Regenerate Now")
        } StyledButton{
            buttonText: qsTr("Regenerate This orbiter")
        }
    }

}
