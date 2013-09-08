import QtQuick 1.0

import "../../../skins-common/lib/handlers"


Row{
    id:nav_row
    height: scaleY(8)
    width:parent.width
    property alias navigation:nav
    property string defaultSource:"ScenarioComponent.qml"
    property string navSource:"ScenarioComponent.qml"
    spacing:scaleX(2)
    Loader{
        id:nav
        source:navSource
        height: parent.height
        width: parent.width*.75
    }
    StyledButton{
        buttonText.text:"Advanced"
        opacity: manager.currentScreen === "Screen_1.qml" ? 1 : 0
        onActivated: manager.gotoQScreen("Screen_44.qml")

    }
    StyledButton {
        id: exit_label
        buttonText.text: qsTr("Exit")
        hitArea.onReleased: manager.exitApp()
        opacity:manager.currentScreen ==="Screen_1.qml" ? 1 : 0
    }
    StyledButton {
        id: home_label
        buttonText.text: qsTr("Home")
        hitArea.onReleased: manager.gotoQScreen("Screen_1.qml")
        opacity: manager.currentScreen !=="Screen_1.qml" ? 1 : 0
    }
    StyledButton{
        id:media_goback
        buttonText.text: "Back"
        hitArea.onReleased:
        {
            if(manager.i_current_mediaType !== 5){
                manager.goBackGrid();
            }
            else{
                
                pageLoader.item.state="selection"
                manager.goBackGrid()
                mediatypefilter.reset()
            }
            
            
        }
        visible: manager.currentScreen==="Screen_47.qml"
    }
}
