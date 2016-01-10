import QtQuick 2.1
import "../components"
Item{
    id:filterButton
    height: scaleY(8)
    width: scaleX(15)
    focus: false
    signal moveUp()
    signal moveDown()
    signal select()
    property int rowindex:-1
    property string text:""
    onActiveFocusChanged: console.log( text+ " active focus is " + activeFocus)

    Rectangle{
        anchors.fill: parent
        color:"black"
        opacity:.65
        radius:5
        border.color: "white"
        border.width: filterButton.activeFocus ? 3 : 0
    }
    StyledText{
        id:label
        text:filterButton.text
        anchors.centerIn: parent
        color:"White"
        font.bold:filterButton.activeFocus
    }
    Keys.onLeftPressed: filtering.moveFocus(rowindex-1)
    Keys.onRightPressed: filtering.moveFocus(rowindex+1)
    Keys.onUpPressed: filterView.decrementCurrentIndex()
    Keys.onDownPressed: filterView.incrementCurrentIndex()
    Keys.onEnterPressed: filter_view.selectItem()
    Keys.onReturnPressed: filter_view.selectItem()
}
