import QtQuick 1.0
import "../../../../skins-common/lib/handlers"
ListView{
    id:floorplanList
    height:scaleY(7)
    width:  (scaleX(10)*8)
    spacing: scaleY(2)
    model:floorplan_pages
    clip:true
    orientation:ListView.Horizontal  

    anchors.verticalCenter: parent.verticalCenter
    property string currentModelName:""
    delegate: StyledButton{
        buttonText: m_description
        onActivated: {
            floorplan_devices.clearAllSelections()
            floorplan_devices.setCurrentPage(m_page)
        }
    }

}
