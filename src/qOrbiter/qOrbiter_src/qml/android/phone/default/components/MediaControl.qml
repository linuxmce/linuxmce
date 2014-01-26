import QtQuick 1.0
import "../../../../skins-common/lib/handlers"
Item{
    anchors.fill: parent

    ListView{
        id:floorplanList
        height:parent.height
        width: parent.width
        spacing: scaleY(2)
        model:control_options
        clip:true
        orientation:ListView.Horizontal

        anchors.verticalCenter: parent.verticalCenter

        delegate: StyledButton{
            buttonText: name
            onActivated: {
//                floorplan_devices.clearAllSelections()
//                floorplan_devices.setCurrentPage(m_page)
            }
        }

    }
    ListModel{
        id:control_options

        ListElement{
            name:"Playlist"
        }
        ListElement{
            name:"Metadata"
        }
        ListElement{
            name:"Advanced"
        }
    }
}


