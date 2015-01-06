import QtQuick 2.2
import "../components"


StyledScreen {
    navigation: "ScenarioComponent.qml"
    keepHeader: true
    Component.onCompleted:{
        current_header_model=scenarios;
        manager.clearDataGrid("Playlist")
        manager.clearDataGrid("sleepingAlarms")

        if(dcenowplaying.b_mediaPlaying){
            manager.setBoundStatus(false)
        }
        setNavigation(nav_row.defaultSource)
        info_panel.state="retracted";

    }
    Item{
        id:home_panel
        height: parent.height - info_panel.height - nav_row.height
        width:parent.width
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: scaleY(8)

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
            cellWidth:manager.isProfile ? scaleX(30) : scaleX(20)
            cellHeight:manager.isProfile ? scaleY(17) : scaleY(22)

            delegate: Item{
                height:scenario_display.cellHeight -5
                width: scenario_display.cellWidth -5

                StyledButton{
                    id:button
                    anchors.fill: parent
                    buttonText: title
                    textSize: manager.isProfile ? scaleY(3) : scaleY(5)
                    hitArea.onReleased: manager.execGrp(params)

                }
            }
        }
    }


}
