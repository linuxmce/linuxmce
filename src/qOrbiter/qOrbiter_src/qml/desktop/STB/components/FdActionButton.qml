import QtQuick 1.1

Item{
    id:fdActionButton
    height: scaleY(10)
    width: scaleX(10)
    property string actionLabel:""
    
    Rectangle{
        anchors.fill: parent
        color:parent.activeFocus ? skinStyle.darkHighlightColor : skinStyle.mainColor
    }
    
    StyledText{
        id:fdActionLabel
        text:fdActionButton.actionLabel
        fontSize: mediumText
    }

    Keys.onLeftPressed: {
        if(fd_action_row.currentItem !==0)
            fd_action_row.currentItem--
    }
    Keys.onRightPressed: {
        if(fd_action_row.currentItem !== fd_action_row.children.length){
                fd_action_row.currentItem++
        }
    }
}
