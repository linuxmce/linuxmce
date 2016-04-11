// import QtQuick 1.1 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import "../js/ComponentLoader.js" as MyJs
//FocusScope{
//    height: scenarios.height
//    width: scenarios.width


//    onFocusChanged: console.log("Focus for Scenario Container is now "+focus)
//    onActiveFocusChanged: console.log("ActiveFocus for Scenario Container is now " +activeFocus)

Rectangle {
    id:scenarios
    width: manager.appWidth
    height:manager.appHeight *.10
    color: "transparent"
    property bool showingMenu
    property double buttonWidth:150
    property double buttonHeight:50
    property double scenarioFontSize:manager.appHeight * .03
    radius: 5

    // Keys.onTabPressed: lmceScenarios.focus= !lmceScenarios.focus
  //  onActiveFocusChanged: activeFocus ? "Scenario Row has active focus" : "Scenario Row lost active focus"
   // onFocusChanged: console.log("Focus inner container is now "+ focus)

    Behavior on opacity{
        PropertyAnimation{
            duration: 500
        }
    }

    ScenarioModelRow {
        id: modelRow
    }

    Image {
        id: rowBg
        source: "../img/thin_bar.png"
        anchors.fill: parent
    }

    anchors.horizontalCenter: parent.horizontalCenter

    ScenarioPopup{
        id:popupHolder
        scenarioModel: ""
        componentHeight: buttonHeight
        componentWidth: buttonWidth
        scenarioName: "Lighting"
    }
FocusScope{
    id:userSelections
    height: scenarios.height
    width: manager.appWidth
    focus:true
    Column{
        height: scenarios.height
        width: manager.appWidth

        ScenariosLayout {
            id: scenariosLayout
        }

        PresenceRow {
            id: presenceRow
        }
    }
}

    //user and location info

    states: [
        State {
            name: "focused"
            when:showingMenu
            PropertyChanges {
                target: scenarios
                border.width: 2
                opacity:1
            }
            PropertyChanges{
                target: userSelections
                focus:true
            }
        },
        State{
            name:"unfocused"
            when:!showingMenu
            PropertyChanges {
                target: scenarios
                border.width: 0
                opacity:0
            }
            PropertyChanges{
                target: userSelections
                focus:false
            }
        }
    ]
}


