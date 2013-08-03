import QtQuick 1.0

import "../../lib/handlers"


Row{
    id:nav_row
    height: scaleY(8)
    width:parent.width
    property alias navigation:nav
    property string navSource:""
    spacing:scaleX(2)
    Loader{
        id:nav
        sourceComponent: ScenarioComponent{}
        height: parent.height
        width: parent.width*.75
    }
    StyledButton{
        buttonText.text:"Advanced"
        visible: manager.currentScreen === "Screen_1.qml"
    }
    StyledButton {
        id: exit_label
        buttonText.text: qsTr("Exit")
        hitArea.onReleased: manager.exitApp()
        visible:manager.currentScreen ==="Screen_1.qml"
    }
    StyledButton {
        id: home_label
        buttonText.text: qsTr("Home")
        hitArea.onReleased: manager.gotoQScreen("Screen_1.qml")
        visible: manager.currentScreen !=="Screen_1.qml"
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
