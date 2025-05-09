import QtQuick 1.1
/*!
 *\class ScenarioButtonHandler
 *\brief Handler Component for Scenario Buttons.
 *
 *\ingroup lib_handlers
 *\bug Key handling not implemented
 *This should be instantiated where you would put the activate button
 *for a scenario. It will tie itself to the model and pass the params itself
 *for the scenario item clicked.
 *
 */
MouseArea{
    id:scenario_handler
    anchors.fill: parent
    onClicked: {
        manager.execGrp(params)
        currentModel = dummy
    }
}
