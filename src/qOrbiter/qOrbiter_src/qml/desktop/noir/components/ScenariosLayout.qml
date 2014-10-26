import QtQuick 1.1
import "../js/ComponentLoader.js" as MyJs
//FocusScope{
//    height: scenarios.height
//    width: scenarios.width


//    onFocusChanged: console.log("Focus for Scenario Container is now "+focus)
//    onActiveFocusChanged: console.log("ActiveFocus for Scenario Container is now " +activeFocus)

Row{
    id:scenariosLayout
    height: parent.height/2
    width: buttonWidth*6
    anchors.horizontalCenter: parent.horizontalCenter
    focus:true
    ListView{
        id:scenarioList
        height: parent.height
        width: buttonWidth*5
        clip:true
        model:modelRow.sModel
        orientation: ListView.Horizontal
        focus: true
        onFocusChanged: console.log("Scenarios Listview has focus")
    }
    
    Rectangle{
        id:advanced
        height: buttonHeight
        width: buttonWidth
        color: "transparent"
        StyledText {
            id: aLabel
            text: qsTr("Advanced")
            anchors.centerIn: parent
            color: "white"
            font.pixelSize: scenarioFontSize
        }
        MouseArea{
            anchors.fill: parent
            onClicked: manager.setCurrentScreen("Screen_44.qml")
        }
    }
}
