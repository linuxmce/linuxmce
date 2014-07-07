import QtQuick 2.1

Item{
    id:fdActionButton
    height: scaleY(10)
    width: scaleX(10)
    property string actionLabel:""
    property int rowIndex:-1
    signal execute()
    onActiveFocusChanged: console.log(actionLabel + " gained active focus")
    
    Rectangle{
        anchors.fill: parent
        color:parent.activeFocus ? appStyle.darkHighlightColor : appStyle.mainColor
    }
    
    StyledText{
        id:fdActionLabel
        text:fdActionButton.actionLabel
        fontSize: mediumText
        color:parent.activeFocus ?  "white" :"grey"
    }



    Keys.onLeftPressed: {
      fd_action_row.moveFocus(rowIndex-1)
    }
    Keys.onRightPressed: {
         fd_action_row.moveFocus(rowIndex+1)
    }
    Keys.onEnterPressed: execute()
    Keys.onReturnPressed: execute()
}
