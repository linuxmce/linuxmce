import QtQuick 2.0
import "../components"
import "../../../../../skins-common/lib/handlers"

Item {
  anchors.fill: parent
    Component.onCompleted:{ current_header_model=scenarios;manager.setBoundStatus(true); info_panel.state="retracted"; nav_row.navSource=nav_row.defaultSource }

    GridView{
        id:scenario_display
        anchors{
            right:parent.right
            left:parent.left
            top:parent.top
            bottom:parent.bottom
            margins: scaleX(5)
        }

        model:current_scenario_model
        cellWidth:scaleX(20)
        cellHeight:scaleY(20)

        delegate: Item{
            height: scaleY(20)
            width: scaleX(20)

            StyledButton{
                id:button
                height: scaleY(20)
                width: scaleX(20)
                buttonText.text: title
                textSize: 38
                hitArea.onReleased: manager.execGrp(params)

            }
        }
    }



}
