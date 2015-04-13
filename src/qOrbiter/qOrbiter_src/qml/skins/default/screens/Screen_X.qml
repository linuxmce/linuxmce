import QtQuick 2.2
import "../"
import "../components"
StyledScreen{

    StyledButton{
        height: Style.appButtonHeight
        width: Style.appButtonWidth *2
        anchors.centerIn: parent
        textSize: Style.appFontSize_title
        buttonText:"Screen Error. Click to return"        
        onActivated: manager.setCurrentScreen(1)
    }

}
