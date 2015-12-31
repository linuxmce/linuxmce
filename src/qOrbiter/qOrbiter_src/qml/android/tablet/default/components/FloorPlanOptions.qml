import QtQuick 1.1

Item{
    id:floorPlan_options
    height: parent.height
    width: parent.width

    Row{
        anchors.fill: parent
        spacing:scaleX(10)
        StyledButton{
            buttonText: "List"
            hitArea.onReleased: pageLoader.item.state="list"
        }
        StyledButton{
            buttonText: "Image"
             hitArea.onReleased: pageLoader.item.state="floorplan"
        }
    }
}
