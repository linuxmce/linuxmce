import QtQuick 2.2

Item {

    height:scaleY(7)
    ListView{
        id:floorplanList
        anchors.left: parent.left
        width:  (scaleX(10)*8)
        height: parent.height
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
}
