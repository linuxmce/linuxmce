import QtQuick 2.2
import "../"
import "../components"
StyledScreen{

    StyledButton{
        height: appStyle.appButtonHeight
        width: appStyle.appButtonWidth *2
        anchors.centerIn: parent
        textSize: appStyle.appFontSize_title
        buttonText:"Screen Error. Click to return"        
        onActivated: manager.setCurrentScreen(1)
    }

}
