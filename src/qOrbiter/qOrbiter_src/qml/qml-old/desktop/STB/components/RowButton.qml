import QtQuick 1.1

Item {
    id:button
    height: scaleY(10)
    width: scaleX(18)
    focus: false
    property alias row:button.parent
    property int rowIndex:-1
    property string text:""

    signal moveUp()
    signal moveDown()
    signal select()

    Rectangle{
        anchors.fill: parent
        color:button.activeFocus ? skinstyle.darkHighlightColor : skinstyle.lightHighlightColor
        border.width: button.activeFocus ? 2 : 0
        opacity: button.activeFocus ? .90 : .65
        radius: 5
    }


    StyledText{
        id:label
        text:button.text
        anchors.centerIn: parent
        color:"White"
        font.bold:button.activeFocus
    }
    Keys.onLeftPressed: row.moveFocus(rowIndex-1)
    Keys.onRightPressed: row.moveFocus(rowIndex+1)
    Keys.onEnterPressed: select()
    Keys.onReturnPressed: select()
    onActiveFocusChanged: {
        if(activeFocus)
            console.log(button.text + "gained active focus")
    }
}
