import QtQuick 2.1
import org.linuxmce.enums 1.0
Item{
    id:focusButton
    height: scaleY(10)
    width: scaleX(18)
    focus: false
    signal moveUp()
    signal moveDown()
    signal select()
    property bool hasParentModel:false
    property int rowIndex:-1
    property string text:""
    property alias fontSize:label.fontSize
    property alias textObj:label
    onActiveFocusChanged: {console.log( text+ " active focus is " + activeFocus); focusedItem = focusButton}

    function processRemoteCommand(command, ident){
        console.log("FocusButton::Processing command #"+command+"-"+ident)
        switch(command){
        case RemoteCommands.MoveLeft:
            parent.moveFocus(rowIndex-1)
            break;

        case RemoteCommands.MoveRight:
            parent.moveFocus(rowIndex+1)
            break;

        case RemoteCommands.EnterGo:
            if(hasParentModel)
                parent.selectItem()
            else select()
            break;

        default:
            console.log("FocusButton::Failed to process command #"+command+"-"+ident)
            break;
        }
    }


    Rectangle{
        anchors.fill: parent
        color:"black"
        opacity:.65
        radius:5
        border.color: "white"
        border.width: focusButton.activeFocus ? 3 : 0
    }
    Rectangle{
        anchors.fill: parent
        gradient: appStyle.buttonGradient
        radius:5
    }
    StyledText{
        id:label
        text:focusButton.text
        anchors.centerIn: parent
        color:"White"
        font.bold:focusButton.activeFocus
    }
    Keys.onLeftPressed: parent.moveFocus(rowIndex-1)
    Keys.onRightPressed: parent.moveFocus(rowIndex+1)
    //Keys.onUpPressed: filterView.decrementCurrentIndex()
    //Keys.onDownPressed: filterView.incrementCurrentIndex()
    Keys.onEnterPressed: {
        if(hasParentModel)
            parent.selectItem()
        else select()
    }
    Keys.onReturnPressed: {
        if(hasParentModel)
            parent.selectItem()
        else select()
    }
    MouseArea{
        anchors.fill: parent
        onClicked: {
            if(hasParentModel)
                parent.selectItem
            else
                select()
        }
    }
}
