import QtQuick 2.0
import "../"
import "../components"
StyledScreen{

    StyledButton{
        anchors.centerIn: parent
        buttonText:"Screen Error. Click to return"
        onActivated: manager.setCurrentScreen(1)
    }

}
