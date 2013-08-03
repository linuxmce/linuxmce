import QtQuick 1.0
import "../components"
import "../../lib/handlers"

Item {
    height:manager.appHeight
    width:manager.appWidth
    Component.onCompleted:{ current_header_model=scenarios;manager.setBoundStatus(true) }
    Item{
        id:home_panel
        height: parent.height - info_panel.height - nav_row.height
        width:parent.width
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: scaleY(8)

        GridView{
            id:scenario_display
            height:parent.height *.75
            width: parent.width *.75
            anchors.centerIn: parent
            model:current_scenario_model
            cellWidth:scaleX(20)
            cellHeight:scaleY(12)

            delegate: Item{
                height: childrenRect.height
                width: childrenRect.width
                StyledButton{
                    id:button
                    buttonText.text: title
                    textSize: 38
                    hitArea.onReleased: manager.execGrp(params)
                }
            }
        }
    }


}
